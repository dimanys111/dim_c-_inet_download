#include "stream.h"

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QThread>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
}

using namespace std;

static AVFormatContext* ifmt_ctx;
static AVFormatContext* ofmt_ctx;

int64_t old_dts[3] = { 0, 0, 0 };

int64_t nom_zap = 0;

static int open_input_file(const char* filename)
{
    int ret;

    ifmt_ctx = NULL;
    if ((ret = avformat_open_input(&ifmt_ctx, filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }
    return 0;
}

static int open_output_file(const char* filename_, QString name_mod, QString& out_fn)
{
    AVStream* out_stream;
    AVStream* in_stream;

    int ret;
    unsigned int i;

    QFile f(filename_);
    QFileInfo fi(f);
    QDateTime date_time = QDateTime::currentDateTime();
    auto d_t = date_time.toString("dd_MM_yyyy_hh_mm_ss");
    QFile f_out(QString(fi.absolutePath() + "/" + name_mod + "_%1.avi").arg(d_t));
    out_fn = f_out.fileName();
    auto a1 = out_fn.toStdString();
    auto filename = a1.c_str();

    avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", nullptr);
    if (!ofmt_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
        return AVERROR_UNKNOWN;
    }

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        old_dts[i] = 0;
        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
            return AVERROR_UNKNOWN;
        }

        in_stream = ifmt_ctx->streams[i];

        ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Copying parameters for stream #%u failed\n", i);
            return ret;
        }
        out_stream->time_base = in_stream->time_base;
    }
    av_dump_format(ofmt_ctx, 0, filename, 1);

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", filename);
            return ret;
        }
    }

    /* init muxer, write output file header */
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
        return ret;
    }

    return 0;
}

void Stream::start_slot(QString in_f, QString name_mod)
{
    QFile f(in_f);
    QFileInfo fi(f);
    auto nom_z = fi.baseName().split("_").at(1).toInt();
    if (nom_zap > 0 && nom_zap != nom_z) {
        end_slot();
    }
    nom_zap = nom_z;

    qDebug() << in_f;
    AVPacket packet = { .data = NULL, .size = 0 };
    unsigned int stream_index;

    if (open_input_file(in_f.toStdString().c_str()) < 0)
        goto end;

    if (!ofmt_ctx) {
        QString s;
        if (open_output_file(in_f.toStdString().c_str(), name_mod, s) < 0)
            goto end;
        emit emit_out_file(s);
    }
    /* read all packets */
    while (1) {
        if (av_read_frame(ifmt_ctx, &packet) < 0)
            break;
        stream_index = packet.stream_index;
        av_log(NULL, AV_LOG_DEBUG, "Demuxer gave frame of stream_index %u\n",
            stream_index);

        /* remux this frame without reencoding */
        av_packet_rescale_ts(&packet,
            ifmt_ctx->streams[stream_index]->time_base,
            ofmt_ctx->streams[stream_index]->time_base);
        auto dts = packet.dts;

        if (dts > old_dts[stream_index]) {
            old_dts[stream_index] = dts;
        } else {
            if ((old_dts[stream_index] - dts) < 5) {
                qDebug() << dts;
                qDebug() << dts << old_dts[stream_index] << "xxx";
                old_dts[stream_index] = dts;
            } else {
                qDebug() << dts << old_dts[stream_index] << "yyy";
            }
        }
        av_interleaved_write_frame(ofmt_ctx, &packet);
        av_packet_unref(&packet);
    }
end:
    av_packet_unref(&packet);
    avformat_close_input(&ifmt_ctx);
    QFile(in_f).remove();
}

void Stream::end_slot()
{
    if (ofmt_ctx) {
        av_write_trailer(ofmt_ctx);
        if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&ofmt_ctx->pb);
        avformat_free_context(ofmt_ctx);
        ofmt_ctx = NULL;
    }
}

Stream::Stream()
{
    ofmt_ctx = NULL;
    moveToThread(new QThread);
    thread()->start();
}

Stream::~Stream()
{
    thread()->exit();
    thread()->wait();
    thread()->deleteLater();
}
