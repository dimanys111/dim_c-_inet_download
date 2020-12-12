#include "mainwindow.h"
#include "QResizeEvent"
#include "QTimer"
#include "ui_mainwindow.h"
#include "work_bonga.h"
#include <stdio.h>
#include <stdlib.h>

#include "QDesktopServices"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , robot(this)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMaxSlid(robot.KolIspWork);
    ui->label->setMinimumHeight(0);
    connect(&robot, &Robot::emit_set_label, this, &MainWindow::set_label);
    connect(&robot, &Robot::emit_set_le, this, &MainWindow::set_le);
    connect(this, &MainWindow::end_emit, &stream, &Stream::end_slot);
    connect(&stream, &Stream::emit_out_file, this, &MainWindow::slot_set_vid_file_name);
}

void MainWindow::slot_set_vid_file_name(QString s)
{
    s_vid_tek = s;
}

void MainWindow::set_label(QByteArray i)
{
    ui->label_2->setText(i);
}

void MainWindow::set_le(QString i)
{
    ui->lineEdit_2->setText(i);
}

void MainWindow::on_pushButton_clicked()
{
    robot.add_sl_url(ui->lineEdit->text());
}

MainWindow::~MainWindow()
{
}

void MainWindow::setMaxSlid(int i)
{
    ui->horizontalSlider->setMaximum(i);
    ui->horizontalSlider->setValue(i);
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    if (!pix.isNull()) {
        resizeImage(e);
    }
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
#ifdef Q_OS_WIN32
    QUrl url(S_tek);
#endif

#ifdef Q_OS_LINUX
    QUrl url;
    if (S_tek.indexOf("http") == 0)
        url.setUrl(S_tek);
    else
        url.setUrl("file://" + S_tek);
#endif
    QDesktopServices::openUrl(url);
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    QUrl url("https://rt.bongacams10.com/" + name_mod);
    QDesktopServices::openUrl(url);
}

void MainWindow::resizeImage(QResizeEvent* e)
{
    int h = e->size().height() - e->oldSize().height();
    int w = e->size().width() - e->oldSize().width();
    if (pix.height() > ui->label->height())
        p = pix.scaledToHeight(ui->label->height() + h - 1);
    else
        p = pix;
    if (p.width() > ui->label->width())
        p = pix.scaledToWidth(ui->label->width() + w - 1);
    ui->label->setPixmap(p);
}

void MainWindow::on_pushButton_2_clicked()
{
    robot.pausa = !robot.pausa;
    if (robot.pausa)
        ui->pushButton_2->setText("Play");
    else
        ui->pushButton_2->setText("Stop");
}

void MainWindow::setImage(QPixmap pp, QString s)
{
    S_tek = s;
    pix = pp;
    if (pix.height() > ui->label->height())
        p = pix.scaledToHeight(ui->label->height());
    else
        p = pix;
    if (p.width() > ui->label->width())
        p = pix.scaledToWidth(ui->label->width());
    ui->label->setPixmap(p);
}

int nom = 1;

void MainWindow::on_pushButton_3_clicked()
{
    nom++;
    robot.clear_sl_url();
    ui->pushButton_3->setText(QString().number(nom));
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    robot.setMaxIspWork(position);
}

void MainWindow::on_pb_start_bonga_clicked()
{
    if (name_mod != ui->lineEdit->text()) {
        name_mod = ui->lineEdit->text();
        setWindowTitle(name_mod);
        emit robot.add_bonga(name_mod);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
#ifdef Q_OS_WIN32
    QUrl url(s_vid_tek);
#endif

#ifdef Q_OS_LINUX
    QUrl url;
    if (s_vid_tek.indexOf("http") == 0)
        url.setUrl(s_vid_tek);
    else
        url.setUrl("file://" + s_vid_tek);
#endif
    QDesktopServices::openUrl(url);
}

void MainWindow::on_pushButton_5_clicked()
{
    QDesktopServices::openUrl(QUrl("file://" + robot.dir_bonga + name_mod));
}
