#include "player.h"

#include <QDebug>
#include <QThread>

int num_bytesBUF;

using namespace std;

Player* Player::instance = 0;
Player* Player::get_instance()
{
    if (instance == 0)
        instance = new Player();
    return instance;
}

void Player::run(std::string str)
{
    this->video_addr = str;
    QMetaObject::invokeMethod(this, &Player::run_, Qt::QueuedConnection);
}

void Player::run_()
{
    if (open() == 0 && malloc() == 0) {
        display_video();
    } else {
        clear();
    }
}

int Player::open()
{
    // open video
    int res = avformat_open_input(&pFormatCtx, video_addr.c_str(), NULL, NULL);
    // check video
    if (res != 0) {
        return -1;
    }
    // get video info
    res = avformat_find_stream_info(pFormatCtx, NULL);
    if (res < 0) {
        return -1;
    }
    // get video stream
    videoStream = get_video_stream();
    if (videoStream == -1) {
        return -1;
    }
    // open
    res = read_audio_video_codec();
    if (res < 0) {
        return -1;
    }
    return 0;
}

/*
Acquires video stream
*/
int Player::get_video_stream(void)
{
    int videoStream = -1;
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            videoStream = i;
    }
    pCodecParameters = pFormatCtx->streams[videoStream]->codecpar;
    return videoStream;
}

/*
Reads audio and video codec
*/
int Player::read_audio_video_codec(void)
{
    auto pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        return -1;
    }
    int res = avcodec_parameters_to_context(pCodecCtx, pCodecParameters);
    if (res < 0) {
        return -1;
    }
    res = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (res < 0) {
        return -1;
    }
    return 0;
}

/*
Alloc memory for the display
*/
int Player::malloc(void)
{
    pFrame = av_frame_alloc();
    if (pFrame == NULL)
        return -1;
    resize = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
        AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);
    if (resize == NULL)
        return -1;
    frameFullScreen = av_frame_alloc();
    if (frameFullScreen == NULL)
        return -1;
    num_bytesBUF = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height, 1);
    buf = (uint8_t*)av_malloc((num_bytesBUF) * sizeof(uint8_t));
    if (buf == NULL)
        return -1;
    int res = av_image_fill_arrays(frameFullScreen->data, frameFullScreen->linesize, buf, AV_PIX_FMT_RGBA,
        pCodecCtx->width, pCodecCtx->height, 1);
    if (res < 0) {
        return -1;
    }
    return 0;
}

/*
Read frames and display
*/
int Player::display_video(void)
{
    AVPacket packet;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStream) {
            avcodec_send_packet(pCodecCtx, &packet);
            avcodec_receive_frame(pCodecCtx, pFrame);
            sws_scale(resize, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, frameFullScreen->data, frameFullScreen->linesize);
            QImage img(buf, pCodecCtx->width, pCodecCtx->height, QImage::Format_RGBA8888);
            img = img.scaled(img.width() / 2, img.height() / 2);
            emit emit_img(img);
            av_frame_unref(pFrame);
        }
        av_packet_unref(&packet);
    }
    clear();
    return 1;
}

void Player::clear()
{
    // close context info
    if (pFormatCtx) {
        avformat_close_input(&pFormatCtx);
    }
    pFormatCtx = nullptr;
    // Close the codecs
    if (pCodecCtx) {
        avcodec_close(pCodecCtx);
        avcodec_free_context(&pCodecCtx);
    }
    pCodecCtx = nullptr;
    // Free the YUV frame
    if (pFrame)
        av_frame_free(&pFrame);
    pFrame = nullptr;
    if (resize)
        sws_freeContext(resize);
    resize = nullptr;
    if (frameFullScreen)
        av_frame_free(&frameFullScreen);
    frameFullScreen = nullptr;
    av_free(buf);
    buf = nullptr;
}

Player::Player()
{
    moveToThread(new QThread);
    thread()->start();
}

Player::~Player()
{
}
