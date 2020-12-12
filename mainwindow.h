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
    ~MainWindow();
    void setMaxSlid(int i);

private:
    QString name_mod;
    QString S_tek;
    QString s_vid_tek;
    QPixmap pix;
    QPixmap p;
    QTimer timer;
    Ui::MainWindow* ui;
    void resizeImage(QResizeEvent* e);

protected:
    void resizeEvent(QResizeEvent* e);
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
signals:
    void start_emit(QString in_f);
    void end_emit();
public slots:
    void setImage(QPixmap pp, QString s);
    void set_label(QByteArray i);
    void set_le(QString i);
    void slot_set_vid_file_name(QString s);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_horizontalSlider_sliderMoved(int position);
    void on_pb_start_bonga_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
};

#endif // MAINWINDOW_H
