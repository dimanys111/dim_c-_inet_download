#ifndef PLAYER_H
#define PLAYER_H

#include <QImage>
#include <QObject>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libpostproc/postprocess.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

class Player : public QObject {

    Q_OBJECT

public:
    static Player* get_instance();

    void run(std::string str);
    void clear();

private:
    static Player* instance;
    Player();
    ~Player();
    int open();

    int malloc(void);
    int display_video(void);

    int get_video_stream(void);

    int read_audio_video_codec(void);

    std::string video_addr;

    int videoStream = 0;

    AVFormatContext* pFormatCtx = NULL;

    AVCodecParameters* pCodecParameters = NULL;

    AVCodecContext* pCodecCtx = NULL;

    AVFrame* pFrame = NULL;

    uint8_t* buf = NULL;
    AVFrame* frameFullScreen = NULL;
    struct SwsContext* resize = NULL;

signals:
    void emit_img(QImage img);
private slots:
    void run_();
};

#endif // PLAYER_H
