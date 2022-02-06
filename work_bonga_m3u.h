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

class Work_Bonga_M3U : public QObject {
    Q_OBJECT
public:
    Work_Bonga wb;
    ~Work_Bonga_M3U();
    Work_Bonga_M3U();
    void setStop();
    QString name_mod;
    QString funZap(QByteArray& content, QString fileName);

private:
    QString vsid_str;
    int delta { 1500 };
    int n_last { -1 };
    QString url_chunks_m3u8;
    QNetworkAccessManager Mymanager;
    void slot_start_first();
private slots:
    void nach();
    void replyFinished();
    void replyFinished_();
public slots:
    void slot_set_model(QString s);
    void slot_start();
signals:
    void emit_set_label(QByteArray i);
    void emit_set_le(QString i);
};

#endif // Work_Bonga_M3U_H
