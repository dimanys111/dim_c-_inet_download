#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "robot.h"
#include "stream.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    Stream stream;
    int Skor;
    QMutex m;
    Robot robot;
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QString name_mod;
    QString s_vid_tek;
    QPixmap pix;
    Ui::MainWindow* ui;
    void resizeImage(QResizeEvent* e);

protected:
    void resizeEvent(QResizeEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* event);
signals:
    void start_emit(QString in_f);
public slots:
    void set_label(QByteArray ba);
    void set_le(QString i);
    void slot_set_vid_file_name(QString s);
    void setImage(QImage img);
private slots:
    void on_pb_start_bonga_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
};

#endif // MAINWINDOW_H
