#ifndef ROBOT_H
#define ROBOT_H

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
    static QString dir_bonga;
    Robot(MainWindow* mw);
    Work_Bonga_M3U wb_m3u;

signals:
    void add_bonga(QString s);
};

#endif // ROBOT_H
