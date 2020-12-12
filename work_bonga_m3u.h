#ifndef Work_Bonga_M3U_H
#define Work_Bonga_M3U_H

#include "work_bonga.h"
#include <QMutex>
#include <QPixmap>
#include <QThread>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

class Robot;
class Work_Bonga_M3U : public QObject {
    Q_OBJECT
public:
    ~Work_Bonga_M3U();
    Work_Bonga_M3U(Robot* r);
    void setStop();
    QString name_mod;
    Robot* robot;
    QString funZap(QByteArray& content, QString fileName);

private:
    Work_Bonga wb;
    QString vsid_str;
    int delta { 1500 };
    int n_last { -1 };
    volatile bool stop { false };
    QString url_chunks_m3u8;
    QThread* Mythread;
    QNetworkAccessManager Mymanager;
    QNetworkAccessManager Mymanager_Post;
    void slot_start_first();
signals:
    void MyemitIMG(QPixmap pp, QString s);
private slots:
    void nach();
    void replyFinished();
    void replyFinished_();
public slots:
    void slot_set_model(QString s);
    void slot_start();
};

#endif // Work_Bonga_M3U_H
