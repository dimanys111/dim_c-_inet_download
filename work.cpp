#include "work.h"
#include "QDir"
#include "QFile"
#include "math.h"
#include "work.h"
#include <QList>
#include <QUrl>
#include <ctime>
#include <htmlcxx/html/Node.h>
#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/tree.h>

#include "mainwindow.h"
#include "robot.h"

using namespace htmlcxx;
using namespace std;

int Work::getSlSize()
{
    return sl.size();
}

void Work::addSl(QString s)
{
    sl.append(s);
}

Work::~Work()
{
    thread()->deleteLater();
}

Work::Work()
{
    moveToThread(new QThread());
    m_manager.moveToThread(thread());
    timer.moveToThread(thread());
    thread()->start();
    connect(&timer, &QTimer::timeout, this, &Work::timeout);
    QMetaObject::invokeMethod(this, &Work::nach, Qt::QueuedConnection);
}

void Work::nach()
{
    if (Robot::stop) {
        sl.clear();
        return;
    }
    auto [empty, s] = sl.get_empty_size_string();
    if (!empty) {
        QUrl url(s);
        dozap = false;
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

        reply_curent = m_manager.get(request);
        timer.start(30000);
        connect(reply_curent, &QNetworkReply::finished,
            this, &Work::replyFinished);

        connect(reply_curent, &QNetworkReply::readyRead,
            this, &Work::downloadProgress);
    } else {
        QThread::msleep(1000);
        QMetaObject::invokeMethod(this, &Work::nach, Qt::QueuedConnection);
    }
}

void Work::timeout()
{
    reply_curent->abort();
}

void Work::setStop()
{
    thread()->exit();
    //thread()->wait();
}

void prov_s1(QString host, QString url, QString& s1)
{
    if (s1.indexOf("//") == 0) {
        s1.replace("//", url.mid(0, url.indexOf("://") + 3));
    } else {
        int h = s1.indexOf("https://");
        if (h > 0) {
            s1.remove(0, h);
            h = 0;
        }
        if (h == -1) {
            h = s1.indexOf("http://");
        }
        if (h > 0) {
            s1.remove(0, h);
            h = 0;
        }
        if (h != 0) {
            if (s1.at(0) == '/') {
                s1 = url.mid(0, url.indexOf("://") + 3) + host + s1;
            } else {
                if (s1.indexOf("..") == 0) {
                    s1 = url.mid(0, url.indexOf("://") + 3) + host + s1.remove(0, 2);
                } else {
                    if (s1.indexOf(".") == 0) {
                        s1.remove(0, 1);
                        if (s1.indexOf("/") == 0)
                            s1.remove(0, 1);
                        s1 = url.replace(url.lastIndexOf("/") + 1, url.size(), s1);
                    } else {
                        if (s1.indexOf("?") == 0) {
                            if (url.indexOf("?") > 0) {
                                s1 = url.replace(url.lastIndexOf("?"), url.size(), s1);
                            } else {
                                s1 = url + s1;
                            }
                        } else {
                            int n1 = url.indexOf("//");
                            n1 = url.indexOf("/", n1 + 2);
                            n1 = url.indexOf("/", n1 + 1);
                            s1 = url.replace(n1 + 1, url.size(), s1);
                        }
                    }
                }
            }
        }
    }
}

void Work::obrabotka(QNetworkReply* reply)
{
    QByteArray content = reply->readAll();
    int siz = content.size();

    QString ur = reply->url().toString();
    QString host = reply->url().host();
    if (siz > 0) {
        int n = 0;
        while (content.at(n) == (char)13 || content.at(n) == (char)10 || content.at(n) == (char)32
            || content.at(n) == (char)239 || content.at(n) == (char)187 || content.at(n) == (char)191) {
            n++;
            if (content.size() - n == 0) {
                n = -1;
                break;
            }
        }
        if (n > -1) {
            if (siz > 1000000 || content.at(n) != '<') {
                if (siz > 100000) {
                    funZap(content, reply->url().toString());
                    QPixmap pp;
                    pp.loadFromData(content);
                    if (!pp.isNull() && pp.height() > 100 && pp.width() > 100) {
                        emit emit_pixmap(pp, reply->url().toString());
                    }
                }
            } else {
                QStringList sl_img;
                QStringList sl_a;

                HTML::ParserDom parser;
                tree<HTML::Node> dom = parser.parseTree(content.toStdString());
                tree<HTML::Node>::iterator it = dom.begin();
                tree<HTML::Node>::iterator end = dom.end();

                for (; it != end; ++it) {
                    if (it->tagName() == "a") {
                        it->parseAttributes();
                        string sss = it->attribute("href").second;
                        QString s1 = QString().fromStdString(sss);
                        QString url = ur;
                        int ind;
                        ind = s1.indexOf("#");
                        if (ind > -1) {
                            s1.remove(ind, s1.length());
                        }
                        ind = s1.indexOf("javascript:");
                        if (ind > -1) {
                            s1.remove(ind, s1.length());
                        }
                        ind = s1.indexOf("mailto:");
                        if (ind > -1) {
                            s1.remove(ind, s1.length());
                        }
                        ind = s1.indexOf("tel:");
                        if (ind > -1) {
                            s1.remove(ind, s1.length());
                        }
                        if (s1.length() > 0) {
                            prov_s1(host, url, s1);
                        }
                        if (s1.length() > 8) {
                            sl_a.append(s1);
                        }
                    }
                    if (it->tagName() == "img") {
                        QString url = ur;
                        it->parseAttributes();
                        QString s2 = QString().fromStdString(it->attribute("style").second);
                        QString s1 = QString().fromStdString(it->attribute("src").second);
                        if (s2.contains("background-image: url(")) {
                            s1 = s2.remove("background-image: url(").remove(");");
                        }
                        if (s1.length() > 0 && s1.indexOf("data:") != 0) {
                            prov_s1(host, url, s1);
                        } else
                            s1 = "";
                        if (s1.length() > 8) {
                            sl_img.append(s1);
                        }
                    }
                }
                dom.clear();
                sl_a.removeDuplicates();
                sl_img.removeDuplicates();
                for (int i = 0; i < sl_a.size(); i++) {
                    Robot::add_sl_url(sl_a.at(i));
                }
                for (int i = 0; i < sl_img.size(); i++) {
                    Robot::add_sl_url_img(sl_img.at(i));
                }
            }
        }
    }
}

void Work::replyFinished()
{
    if (timer.isActive()) {
        timer.stop();
    }
    QNetworkReply* reply = (QNetworkReply*)sender();
    if (Robot::stop) {
        sl.clear();
        return;
    }
    if (reply->hasRawHeader("Location")) {
        QString s1 = reply->rawHeader("Location");
        if (s1.length() > 8) {
            Robot::add_sl_url(s1);
        }
    }

    if (reply->error() == QNetworkReply::NoError) {
        // Получаем содержимое ответа
        if (dozap) {
            QByteArray content = reply->readAll();
            funZap(content, reply->url());
        } else {
            obrabotka(reply);
        }
    }

    if (reply->error() != QNetworkReply::NoError) {
        //qDebug() << "YYY=" << reply->url();
        //qDebug() << reply->error();
    }
    reply->deleteLater();
    QMetaObject::invokeMethod(this, &Work::nach, Qt::QueuedConnection);
}

void Work::funZap(QByteArray& content, const QUrl& url)
{
    QString put = url.toString();
    put.remove("http://").remove("https://");
    int n = put.lastIndexOf("/");
    if (n > -1)
        put.remove(n + 1, put.size());
    else
        put = put + "/";
    put.replace(".", "_");

    QString fileName = url.fileName();
    if (fileName == "")
        fileName = randSimv();

    QDir d;
    bool b = d.mkpath(Robot::dir_web + put);
    if (!b) {
        qDebug() << put;
    }
    QFile f(Robot::dir_web + put + fileName);
    if (dozap) {
        if (f.open(QIODevice::Append)) {
            f.write(content);
            f.close();
        }
    } else {
        if (f.open(QIODevice::WriteOnly)) {
            f.write(content);
            f.close();
        }
        QString s = f.fileName();
        int i = s.lastIndexOf(".");
        if (i > 0) {
            s.remove(i, s.size());
        }
        f.setFileName(s + ".txt");
        if (f.open(QIODevice::WriteOnly)) {
            f.write(url.toString().toUtf8());
            f.close();
        }
    }
}

void Work::downloadProgress()
{
    if (timer.isActive()) {
        timer.stop();
    }
    QNetworkReply* reply = (QNetworkReply*)sender();
    if (Robot::stop) {
        sl.clear();
        return;
    }
    if (reply->size() > 5000000) {
        QByteArray content = reply->readAll();
        funZap(content, reply->url());
        dozap = true;
    }
}
