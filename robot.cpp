#include "robot.h"
#include "mainwindow.h"
#include <QApplication>

QString Robot::dir_bonga;

Robot::Robot(MainWindow* mw)
{
    dir_bonga = qApp->applicationDirPath() + "/bonga/";
    connect(&wb_m3u.wb, &Work_Bonga::emit_part_vid, &mw->stream, &Stream::start_slot);
    connect(&wb_m3u, &Work_Bonga_M3U::emit_set_label, mw, &MainWindow::set_label);
    connect(&wb_m3u, &Work_Bonga_M3U::emit_set_le, mw, &MainWindow::set_le);

    connect(this, &Robot::add_bonga, &wb_m3u, &Work_Bonga_M3U::slot_set_model);
}
