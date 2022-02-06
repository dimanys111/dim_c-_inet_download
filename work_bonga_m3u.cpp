#include "work_bonga_m3u.h"
#include "QDir"
#include "QFile"
#include "math.h"
#include "work.h"
#include <QBuffer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QUrl>
#include <QUrlQuery>
#include <ctime>
#include <htmlcxx/html/Node.h>
#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/tree.h>

#include "mainwindow.h"
#include "robot.h"

using namespace htmlcxx;
using namespace std;

Work_Bonga_M3U::~Work_Bonga_M3U()
{
    thread()->deleteLater();
}

Work_Bonga_M3U::Work_Bonga_M3U()
    : wb(this)
{
    moveToThread(new QThread());
    Mymanager.moveToThread(thread());
    thread()->start();
    QTimer::singleShot(delta, this, &Work_Bonga_M3U::nach);

    QMetaObject::invokeMethod(this, &Work_Bonga_M3U::slot_start_first, Qt::DirectConnection);
}

void Work_Bonga_M3U::slot_start_first()
{

    slot_start();

    QTimer::singleShot(30000, this, &Work_Bonga_M3U::slot_start);
}

void Work_Bonga_M3U::nach()
{
    if (Robot::stop) {
        return;
    }

    if (url_chunks_m3u8 != "") {
        QUrl url(url_chunks_m3u8);

        // создаем объект для запроса
        QNetworkRequest request(url);
        request.setRawHeader(
            "Connection",
            "keep-alive");
        request.setRawHeader(
            "Upgrade-Insecure-Requests",
            "1");
        request.setRawHeader(
            "User-Agent",
            "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.71 Safari/537.36");
        request.setRawHeader(
            "Accept",
            "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
        request.setRawHeader(
            "Accept-Language",
            "ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4");
        // Выполняем запрос, получаем указатель на объект
        // ответственный за ответ
        QNetworkReply* reply = Mymanager.get(request);
        // Подписываемся на сигнал о готовности загрузки
        connect(reply, &QNetworkReply::finished,
            this, &Work_Bonga_M3U::replyFinished);
    }

    QTimer::singleShot(delta, this, &Work_Bonga_M3U::nach);
}

void Work_Bonga_M3U::slot_start()
{
    if (Robot::stop) {
        return;
    }

    if (name_mod != "") {
        QByteArray ba = QString("method=getRoomData&args[]=" + name_mod + "&args[]=&args[]=").toUtf8();

        QDateTime dt(QDateTime::currentDateTime());
        auto time_str = QString::number(dt.toMSecsSinceEpoch());

        auto url = QUrl("https://rt.bongacams21.com/tools/amf.php?x-country=ru&res=475592?" + time_str);
        QNetworkRequest request(url);
        request.setRawHeader("Content-Type",
            "application/x-www-form-urlencoded");
        request.setRawHeader("X-Requested-With",
            "XMLHttpRequest");

        request.setRawHeader("x-ab-split-group",
            "aa6f9c0902ce07fd14fc210c92473c977e07ddea0fe157dbcd90c9521ab5e1e571d8a93f2d7edce3");
        request.setRawHeader("cookie",
            "bonga20120608=2960d2a800a07f1bdc54fe47accdcc71");

        QNetworkReply* reply = Mymanager.post(request, ba);
        connect(reply, &QNetworkReply::finished,
            this, &Work_Bonga_M3U::replyFinished_);
    }
}

void Work_Bonga_M3U::replyFinished_()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    QString s = reply->url().toString();
    qDebug() << s;
    QByteArray content = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(content);
    // Забираем из документа корневой объект
    auto o = document.object();
    QJsonValue root = o.value("localData");
    QJsonValue videoServerUrl = root.toObject().value("videoServerUrl");
    s = videoServerUrl.toString();
    url_chunks_m3u8 = "https:" + s + "/hls/stream_" + name_mod + "/public-aac/stream_" + name_mod + "/chunks.m3u8";
    QJsonValue vsid = root.toObject().value("vsid");
    vsid_str = vsid.toString();
    if (Robot::stop) {
        return;
    }
    reply->deleteLater();
}

void Work_Bonga_M3U::slot_set_model(QString s)
{
    name_mod = s;
    slot_start_first();
}

QString Work_Bonga_M3U::funZap(QByteArray& content, QString fileName)
{
    QDir d;
    QFile f(Robot::dir_bonga + name_mod + "/" + fileName);
    bool b = d.mkpath(Robot::dir_bonga + name_mod);
    if (!b) {
        qDebug() << name_mod;
    }
    if (f.open(QIODevice::ReadWrite)) {
        f.write(content);
        f.close();
    }

    return f.fileName();
}

void Work_Bonga_M3U::replyFinished()
{
    QNetworkReply* reply = (QNetworkReply*)sender();

    if (Robot::stop) {
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        // Получаем содержимое ответа
        QByteArray content = reply->readAll();
        //funZap(content, "chunks.m3u8");
        QBuffer buffer(&content);
        buffer.open(QIODevice::ReadOnly);

        QTextStream in(&buffer);
        QString out;
        while (!in.atEnd()) {
            QString s = in.readLine();
            if (s.at(0) != '#') {
                out = reply->url().toString().remove("chunks.m3u8") + s;
                wb.addSl(out);
                s.remove(".ts");
                auto sl = s.split("_");
                auto n = sl.at(2).toInt();
                if (n_last > 0 && (n - n_last) > 0)
                    delta = n - n_last;
                n_last = n;
            }
        }
        if (out == "") {
            wb.zap_all();
        }
        emit emit_set_le(out);
        emit emit_set_label(content);
    } else {
        qDebug() << reply->url();
        qDebug() << reply->error();
    }

    reply->deleteLater();
}

void Work_Bonga_M3U::setStop()
{
    wb.setStop();
    thread()->exit();
    thread()->wait();
}
