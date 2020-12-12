#ifndef WORK_H
#define WORK_H

#include <QMutex>
#include <QPixmap>
#include <QThread>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

int containsListChar(QList<char*>& list, QString s);
char* StrToChar(QString s);
QString randSimv();

class Robot;
class Work : public QObject {
    Q_OBJECT
public:
    ~Work();
    Work(Robot* rob);
    int getSlSize();
    void addSl(QString s);
    void prov_s1(QString host, QString url, QString& s1);
    void setStop();

private:
    Robot* rob;
    bool stop;
    static QMutex m1;
    QString put;
    qint64 seek;
    bool prow;
    QString fileName;
    bool dozap, prop;
    QString S;
    QStringList Sl;
    QMutex m;
    qint64 BytesReceived;
    qint64 BytesReceivedOld;
    QThread* Mythread;
    QNetworkAccessManager Mymanager;
    void funProw(QNetworkReply* reply);
    void funZap(QByteArray& content);
    void obrabotka(QNetworkReply* reply);

signals:
    void Myemit();
    void MyemitIMG(QPixmap pp, QString s);
private slots:
    void nach();
    void replyFinished();
    void downloadProgress();
};

#endif // WORK_H
