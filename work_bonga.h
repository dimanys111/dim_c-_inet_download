#ifndef WORK_Bonga_H
#define WORK_Bonga_H

#include <QMutex>
#include <QPixmap>
#include <QThread>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

class Work_Bonga_M3U;

class Work_Bonga : public QObject {
    Q_OBJECT
public:
    //static std::chrono::system_clock::time_point lastTime_kadr;
    ~Work_Bonga();
    Work_Bonga(Work_Bonga_M3U* wb);
    void addSl(QString s);
    void setStop();

    void zap_all();

private:
    QStringList list_zap;
    QList<int> list_nom;
    int nom_pot { 0 };
    Work_Bonga_M3U* wb { nullptr };
    QStringList list_all;
    QNetworkAccessManager Mymanager;
    void funZap(QByteArray& content, QString& fileName);

signals:
    void emit_part_vid(QString s, QString name_mod);
    void Myemit(QString url_str);
private slots:
    void nach(QString url_str);
    void replyFinished();
};

#endif // WORK_Bonga_H
