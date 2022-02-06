#include "robot.h"
#include "mainwindow.h"
#include "work.h"
#include <QApplication>

volatile bool Robot::pausa;
QString Robot::dir_web;
QString Robot::dir_bonga;
volatile bool Robot::stop;
StringListThread Robot::sl_url;

Robot::Robot(MainWindow* mw)
    : sl(-1, "")
{
    connect(&wb_m3u.wb, &Work_Bonga::emit_part_vid, &mw->stream, &Stream::start_slot);
    connect(&wb_m3u, &Work_Bonga_M3U::emit_set_label, mw, &MainWindow::set_label);
    connect(&wb_m3u, &Work_Bonga_M3U::emit_set_le, mw, &MainWindow::set_le);
    pausa = false;
    moveToThread(new QThread());
    thread()->start();
    QDir path;
    path.mkpath(qApp->applicationDirPath() + "/zzz");
    dir_web = qApp->applicationDirPath() + "/zzz/";
    dir_bonga = qApp->applicationDirPath() + "/bonga/";
    connect(this, &Robot::add_bonga, &wb_m3u, &Work_Bonga_M3U::slot_set_model);
    for (int i = 0; i < 100; i++) {
        ws << new Work();
        connect(ws.last(), &Work::emit_pixmap, mw, &MainWindow::setPixmap);
    }

    KolIspWork = ws.size();

    QFile f(qApp->applicationDirPath() + "/sl.sl");
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream ts(&f);
        int i = 0;
        while (!ts.atEnd()) {
            i++;
            QString s;
            ts >> s;
            sl.zag(s);
        }
        f.close();
    }

    f.setFileName(qApp->applicationDirPath() + "/sl_url.sl");
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream ts(&f);
        while (!ts.atEnd()) {
            QString s;
            ts >> s;
            sl_url.append_not_mutex(s);
        }
        f.close();
    }

    QMetaObject::invokeMethod(this, &Robot::addWork, Qt::QueuedConnection);
}

Robot::~Robot()
{
    thread()->deleteLater();
}

void Robot::add_sl_url(QString s)
{
    if (!pausa)
        sl_url.append(s);
}

void Robot::add_sl_url_img(QString s)
{
    if (!pausa)
        sl_url.append_first(s);
}

void Robot::setMaxIspWork(int i)
{
    KolIspWork = i;
}

void Robot::clear_sl_url()
{
    sl_url.clear();
}

void Robot::save()
{
    QFile f(qApp->applicationDirPath() + "/sl.sl");
    if (f.open(QIODevice::WriteOnly)) {
        QTextStream ts(&f);
        sl.save_SV(ts);
        ts.flush();
        f.close();
    }
    sl.clear();
    f.setFileName(qApp->applicationDirPath() + "/sl_url.sl");
    if (f.open(QIODevice::WriteOnly)) {
        QTextStream ts(&f);
        sl_url.save_to_ts(ts);
        ts.flush();
        f.close();
    }
    sl_url.clear();
}

void Robot::set_save_bool()
{
    stop = true;
    save();
    int i = 0;
    while (i < ws.size()) {
        ws.at(i)->setStop();
        i++;
    }
    wb_m3u.setStop();
    thread()->exit();
    thread()->wait();
}

void Robot::addWork()
{
    while (true) {
        if (stop)
            return;
        if (!pausa) {
            auto [b, s] = sl_url.get_empty_size_string();
            if (!b) {
                if (!sl.contains(s)) {
                    sl.append(s);
                    while (!b) {
                        int z = KolIspWork;
                        for (int n = 0; n < z; n++) {
                            if (ws.at(n)->getSlSize() < 100) {
                                ws.at(n)->addSl(s);
                                b = true;
                                break;
                            }
                        }
                        if (!b) {
                            QThread::msleep(100);
                        }
                    }
                }
            } else {
                QThread::msleep(1000);
            }
        } else {
            sl.clear();
            sl_url.clear();
            QThread::msleep(2000);
            for (int n = 0; n < ws.size(); n++) {
                if (ws.at(n)->getSlSize()) {
                    qDebug() << n << "=" << ws.at(n)->getSlSize();
                }
            }
            qDebug() << "всего sl=" << sl.size();
            qDebug() << "всего sl_url=" << sl_url.size();
        }
    }
}
