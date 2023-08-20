#include "work_bonga_m3u.h"
#include "QDir"
#include "QFile"
#include "math.h"
#include <QBuffer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QUrl>
#include <QUrlQuery>
#include <ctime>

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
}

bool slot_start_ = false;

void Work_Bonga_M3U::slot_set_model(QString s)
{
    name_mod = s;
    if (!slot_start_)
        slot_start();
}

void Work_Bonga_M3U::slot_start()
{
    slot_start_ = true;
    QByteArray ba = QString("method=getRoomData&args[]=" + name_mod + "&args[]=&args[]=").toUtf8();

    QDateTime dt(QDateTime::currentDateTime());
    auto time_str = QString::number(dt.toMSecsSinceEpoch());

    auto url = QUrl("https://rus.bonga-cams.com/tools/amf.php?res=934305&t=" + time_str);
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type",
        "application/x-www-form-urlencoded");
    request.setRawHeader("X-Requested-With",
        "XMLHttpRequest");
    request.setRawHeader("cookie",
        "bonga20120608=6fe1c1f2baf1d620453b50a1ebecf951");

    QNetworkReply* reply = Mymanager.post(request, ba);
    connect(reply, &QNetworkReply::finished,
        this, &Work_Bonga_M3U::replyFinished_url_chunks_m3u8);
}

bool nach_ = false;

void Work_Bonga_M3U::replyFinished_url_chunks_m3u8()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    QString s = reply->url().toString();
    qDebug() << s;
    QByteArray content = reply->readAll();
    qDebug() << content;
    QJsonDocument document = QJsonDocument::fromJson(content);
    // Забираем из документа корневой объект
    auto o = document.object();
    QJsonValue root = o.value("localData");
    QJsonValue videoServerUrl = root.toObject().value("videoServerUrl");
    s = videoServerUrl.toString();
    url_chunks_m3u8 = "https:" + s + "/hls/stream_" + name_mod + "/public-aac/stream_" + name_mod + "/chunks.m3u8";
    reply->deleteLater();
    if (!nach_)
        nach();
}

void Work_Bonga_M3U::nach()
{
    nach_ = true;
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

    QTimer::singleShot(m_deltaUpdateM3U, this, &Work_Bonga_M3U::nach);
}

void Work_Bonga_M3U::replyFinished()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    if (reply->error() == QNetworkReply::NoError) {
        // Получаем содержимое ответа
        QByteArray content = reply->readAll();
        // funZap(content, "chunks.m3u8");
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
                auto numberChankLast = sl.at(2).toInt();
                if (m_numberChankLast > 0 && (numberChankLast - m_numberChankLast) > 0)
                    m_deltaUpdateM3U = numberChankLast - m_numberChankLast;
                m_numberChankLast = numberChankLast;
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
