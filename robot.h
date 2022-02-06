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
    static volatile bool pausa;
    static QString dir_web;
    static QString dir_bonga;
    static volatile bool stop;
    Robot(MainWindow* mw);
    Work_Bonga_M3U wb_m3u;
    ~Robot();
    static void add_sl_url(QString s);
    static void add_sl_url_img(QString s);
    void setMaxIspWork(int i);
    void set_save_bool();

private:
    static StringListThread sl_url;
    SpisoVetka sl;
    QList<Work*> ws;
signals:
    void add_bonga(QString s);
public slots:
    void clear_sl_url();
    void addWork();
    void save();
};

#endif // ROBOT_H
