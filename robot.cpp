#include "robot.h"
#include "mainwindow.h"
#include "work.h"
#include <QApplication>

Robot::Robot(MainWindow* mw)
    : mw(mw)
    , wb_m3u(this)
{
    pausa = false;
    Mythread = new QThread();
    moveToThread(Mythread);
    Mythread->start();
    QDir path;
    path.mkpath(qApp->applicationDirPath() + "/zzz");
    dir_web = qApp->applicationDirPath() + "/zzz/";
    dir_bonga = qApp->applicationDirPath() + "/bonga/";
    connect(this, &Robot::add_bonga, &wb_m3u, &Work_Bonga_M3U::slot_set_model);
    for (int i = 0; i < 100; i++) {
        ws << new Work(this);
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
            sl_url.append_not_mutex(StrToChar(s));
        }
        f.close();
    }

    QTimer::singleShot(0, this, [&] { addWork(); });
}

Robot::~Robot()
{
}

void Robot::add_sl_url(QString s)
{
    sl_url.add(s);
}

void Robot::add_sl_url_img(QString s)
{
    sl_url.add_first(s);
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
    f.setFileName(qApp->applicationDirPath() + "/sl_url.sl");
    if (f.open(QIODevice::WriteOnly)) {
        QTextStream ts(&f);
        sl_url.save_to_ts(ts);
        ts.flush();
        f.close();
    }
}

void Robot::set_save_bool()
{
    save();
    stop = true;
    Mythread->exit();
    Mythread->wait();
    Mythread->deleteLater();
    int i = 0;
    while (i < ws.size()) {
        ws.at(i)->setStop();
        i++;
    }
    wb_m3u.setStop();
}

int jjjold = 0;

void Robot::addWork()
{
    int jjj = sl_url.size_not_mutex() / 1000000 + 1;
    qDebug() << "jjj=" << jjj;
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
                            if (ws.at(n)->getSlSize() < 10) {
                                ws.at(n)->addSl(s);
                                b = true;
                                break;
                            }
                        }
                        if (!b) {
                            QThread::msleep(1000);
                        }
                    }
                }
            } else {
                QThread::msleep(1000);
            }
        } else {
            QThread::msleep(2000);
            for (int n = 0; n < ws.size(); n++) {
                if (ws.at(n)->getSlSize()) {
                    qDebug() << n << "=" << ws.at(n)->getSlSize();
                }
            }
            qDebug() << "всего sl=" << sl.size();
            qDebug() << "всего sl_url=" << sl_url.size_not_mutex();
        }
    }
}
