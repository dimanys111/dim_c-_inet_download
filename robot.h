#ifndef ROBOT_H
#define ROBOT_H

#include "spisovetka.h"
#include "stringlistthread.h"
#include "work_bonga_m3u.h"
#include <QDir>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMutex>
#include <QObject>

class Work;
class MainWindow;

class Robot : public QObject {
    Q_OBJECT
public:
    int KolIspWork;
    MainWindow* mw;
    volatile bool pausa;
    QString dir_web;
    QString dir_bonga;
    Robot(MainWindow* mw);
    Work_Bonga_M3U wb_m3u;
    ~Robot();
    void add_sl_url(QString s);
    void add_sl_url_img(QString s);
    void setMaxIspWork(int i);

private:
    StringListThread sl_url;
    SpisoVetka sl;
    volatile bool stop { false };
    QThread* Mythread;
    QList<Work*> ws;
    void set_save_bool();
signals:
    void add_bonga(QString s);
    void emit_set_label(QByteArray i);
    void emit_set_le(QString i);
public slots:
    void clear_sl_url();
    void addWork();
    void save();
};

#endif // ROBOT_H
