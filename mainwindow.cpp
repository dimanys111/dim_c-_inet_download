#include "mainwindow.h"
#include "QResizeEvent"
#include "QTimer"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <stdlib.h>

#include "QDesktopServices"

#include "player.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , robot(this)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->setMinimumHeight(0);
    connect(&stream, &Stream::emit_out_file, this, &MainWindow::slot_set_vid_file_name);
    connect(Player::get_instance(), &Player::emit_img, this, &MainWindow::setImage);
}

void MainWindow::slot_set_vid_file_name(QString s)
{
    s_vid_tek = s;
}

void MainWindow::setImage(QImage img)
{
    pix = QPixmap::fromImage(img);
    QPixmap pixmap;
    if (pix.height() > ui->label->height())
        pixmap = pix.scaledToHeight(ui->label->height());
    else
        pixmap = pix;
    if (pixmap.width() > ui->label->width())
        pixmap = pix.scaledToWidth(ui->label->width());
    ui->label->setPixmap(pixmap);
}

void MainWindow::set_label(QByteArray ba)
{
    ui->label_2->setText(ba);
}

void MainWindow::set_le(QString i)
{
    ui->lineEdit_2->setText(i);
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    if (!pix.isNull()) {
        resizeImage(e);
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event)
    QUrl url("https://rus.bonga-cams.com/" + name_mod);
    QDesktopServices::openUrl(url);
}

void MainWindow::resizeImage(QResizeEvent* e)
{
    int h = e->size().height() - e->oldSize().height();
    int w = e->size().width() - e->oldSize().width();
    QPixmap pixmap;
    if (pix.height() > ui->label->height())
        pixmap = pix.scaledToHeight(ui->label->height() + h - 1);
    else
        pixmap = pix;
    if (pixmap.width() > ui->label->width())
        pixmap = pix.scaledToWidth(ui->label->width() + w - 1);
    ui->label->setPixmap(pixmap);
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
    QDesktopServices::openUrl(QUrl("file://" + Robot::dir_bonga + name_mod));
}
