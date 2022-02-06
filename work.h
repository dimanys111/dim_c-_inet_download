#ifndef WORK_H
#define WORK_H

#include "stringlistthread.h"

#include <QMutex>
#include <QPixmap>
#include <QThread>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

QString randSimv();

class Work : public QObject {
    Q_OBJECT
public:
    ~Work();
    Work();
    int getSlSize();
    void addSl(QString s);
    void setStop();

private:
    QNetworkReply* reply_curent { nullptr };
    StringListThread sl;
    QTimer timer;
    QNetworkAccessManager m_manager;
    bool dozap;
    void funZap(QByteArray& content, const QUrl& url);
    void obrabotka(QNetworkReply* reply);
signals:
    void emit_pixmap(QPixmap pp, QString s);
private slots:
    void nach();
    void replyFinished();
    void downloadProgress();
    void timeout();
};

#endif // WORK_H
