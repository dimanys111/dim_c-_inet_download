#include "work_bonga.h"
#include "QDir"
#include "QFile"
#include "math.h"
#include "work.h"
#include "work_bonga_m3u.h"
#include <QUrl>
#include <ctime>
#include <htmlcxx/html/Node.h>
#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/tree.h>

#include "mainwindow.h"
#include "player.h"
#include "robot.h"

using namespace htmlcxx;
using namespace std;

QString randSimv()
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 12; // assuming you want random strings of 12 characters

    QString randomString;
    for (int i = 0; i < randomStringLength; ++i) {
        int index = rand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

void Work_Bonga::addSl(QString s)
{
    if (!list_all.contains(s)) {
        list_all << s;
        emit Myemit(s);
    }
}

Work_Bonga::~Work_Bonga()
{
    Work_Bonga::zap_all();
    thread()->deleteLater();
}

Work_Bonga::Work_Bonga(Work_Bonga_M3U* wb)
    : wb(wb)
{
    moveToThread(new QThread());
    Mymanager.moveToThread(thread());
    connect(this, &Work_Bonga::Myemit, this, &Work_Bonga::nach);
    thread()->start();
}

void Work_Bonga::nach(QString url_str)
{
    if (Robot::stop) {
        return;
    }

    QUrl url(url_str);

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
    connect(reply, &QNetworkReply::finished, this, &Work_Bonga::replyFinished);
}

void Work_Bonga::replyFinished()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    if (Robot::stop) {
        return;
    }
    if (reply->error() == QNetworkReply::NoError) {
        // Получаем содержимое ответа
        QByteArray content = reply->readAll();

        QString fileName = reply->url().fileName();
        if (fileName == "")
            fileName = randSimv();
        funZap(content, fileName);
    } else {
        qDebug() << reply->url();
        qDebug() << reply->error();
    }
    delete reply;
}

void Work_Bonga::setStop()
{
    zap_all();
    thread()->exit();
    thread()->wait();
}

void Work_Bonga::zap_all()
{
    while (!list_zap.empty()) {
        auto nom = std::distance(list_nom.begin(), std::min_element(list_nom.begin(), list_nom.end()));
        emit emit_part_vid(list_zap.at(nom), wb->name_mod);
        list_zap.removeAt(nom);
        list_nom.removeAt(nom);
    }
}

void Work_Bonga::funZap(QByteArray& content, QString& fileName)
{
    QString put = wb->name_mod + "/";
    QDir d;
    bool b = d.mkpath(Robot::dir_bonga + put);
    if (!b) {
        qDebug() << put;
    }
    QFile f(Robot::dir_bonga + put + fileName);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(content);
        f.close();
    }
    Player::get_instance()->run(f.fileName().toStdString());
    auto np = QFileInfo(f).baseName().split("_").at(1).toInt();
    if (nom_pot > 0 && np != nom_pot) {
        zap_all();
    }
    nom_pot = np;
    list_zap << f.fileName();
    list_nom << QFileInfo(f).baseName().split("_").at(3).toInt();
    if (list_nom.size() > 5) {
        auto nom = std::distance(list_nom.begin(), std::min_element(list_nom.begin(), list_nom.end()));
        emit emit_part_vid(list_zap.at(nom), wb->name_mod);
        list_zap.removeAt(nom);
        list_nom.removeAt(nom);
    }
}
