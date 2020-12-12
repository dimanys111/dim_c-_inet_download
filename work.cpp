#include "work.h"
#include "QDir"
#include "QFile"
#include "math.h"
#include "work.h"
#include <QUrl>
#include <ctime>
#include <htmlcxx/html/Node.h>
#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/tree.h>

#include "mainwindow.h"
#include "robot.h"

using namespace htmlcxx;
using namespace std;

QMutex Work::m1;

int containsListChar(QList<char*>& list, QString s)
{
    QByteArray buf = s.toUtf8();
    char* c = buf.data();
    for (int i = 0; i < list.size(); i++) {
        int n = 0;
        bool pr = true;
        char* b = list.at(i);
        while (b[n] != 0 || c[n] != 0) {
            if (b[n] != c[n]) {
                pr = false;
                break;
            }
            n++;
        }
        if (pr) {
            if (b[n] == c[n]) {
                return i;
            }
        }
    }
    return -1;
}

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

int Work::getSlSize()
{
    m.lock();
    int n = Sl.size();
    m.unlock();
    return n;
}

void Work::addSl(QString s)
{
    m.lock();
    Sl.append(s);
    m.unlock();
}

Work::~Work()
{
}

Work::Work(Robot* rob)
    : rob(rob)
{
    stop = false;
    Mythread = new QThread();
    Mymanager.moveToThread(Mythread);
    moveToThread(Mythread);
    connect(Mythread, &QThread::finished, this, &Work::deleteLater);
    connect(Mythread, &QThread::finished, Mythread, &QThread::deleteLater);
    connect(this, &Work::MyemitIMG, rob->mw, &MainWindow::setImage);
    connect(this, &Work::Myemit, this, &Work::nach);
    Mythread->start();
    emit Myemit();
}

void Work::nach()
{
    while (true) {
        if (stop) {
            Sl.clear();
            return;
        }

        m.lock();
        bool b = Sl.isEmpty();
        if (!b) {
            S = Sl.takeFirst();
            m.unlock();

            prop = false;
            seek = 0;
            prow = false;
            dozap = false;
            BytesReceivedOld = 0;
            BytesReceived = 0;

            QUrl url(S);

            put = S;
            put.remove("http://").remove("https://");
            int n = put.lastIndexOf("/");
            if (n > -1)
                put.remove(n + 1, put.size());
            else
                put = put + "/";
            put.replace(".", "_");

            fileName = url.fileName();
            if (fileName != "") {
                //            QFile f(Robot::MW->d+put+fileName);
                //            if(f.exists())
                //            {
                //                seek=f.size();
                //            }
            } else
                fileName = randSimv();

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
            if (seek) {
                QByteArray b = QString("bytes=%1-").arg(seek).toUtf8();
                request.setRawHeader(
                    "range",
                    b);
            }
            request.setRawHeader(
                "Accept-Language",
                "ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4");
            // Выполняем запрос, получаем указатель на объект
            // ответственный за ответ
            QNetworkReply* reply = Mymanager.get(request);
            // Подписываемся на сигнал о готовности загрузки
            connect(reply, &QNetworkReply::finished,
                this, &Work::replyFinished);

            connect(reply, &QNetworkReply::readyRead,
                this, &Work::downloadProgress);
            break;
        } else {
            m.unlock();
            QThread::msleep(1000);
        }
    }
}

void Work::prov_s1(QString host, QString url, QString& s1)
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
    QStringList sl_img;
    QStringList sl_a;

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
            if (content.at(n) != '<') {
                if (siz > 100000) {
                    funZap(content);
                    QPixmap pp;
                    pp.loadFromData(content);
                    if (!pp.isNull() && pp.height() > 100 && pp.width() > 100) {
                        emit MyemitIMG(pp, S);
                    }
                }
            } else {
                string html = content.toStdString();
                HTML::ParserDom parser;
                tree<HTML::Node> dom = parser.parseTree(html);
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
                        if (s1.contains("ru.webcams.travel")) {
                            qDebug() << ur;
                            qDebug() << s1;
                            qDebug() << "";
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
                sl_a.removeDuplicates();
                sl_img.removeDuplicates();
                for (int i = 0; i < sl_a.size(); i++) {
                    rob->add_sl_url(sl_a.at(i));
                }
                for (int i = 0; i < sl_img.size(); i++) {
                    rob->add_sl_url_img(sl_img.at(i));
                }
            }
        }
    }
}

void Work::replyFinished()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    if (stop) {
        Sl.clear();
        return;
    }

    funProw(reply);

    if (reply->hasRawHeader("Location")) {
        QString s1 = reply->rawHeader("Location");
        if (s1.length() > 8) {
            rob->add_sl_url(s1);
        }
    }

    if (reply->error() == QNetworkReply::NoError && !prop) {
        // Получаем содержимое ответа
        if (dozap) {
            QByteArray content = reply->readAll();
            funZap(content);
        } else {
            obrabotka(reply);
        }
    }

    if (reply->error() != QNetworkReply::NoError) {
        Work::m1.lock();
        qDebug() << "XXX=" << S;
        qDebug() << "YYY=" << reply->url();
        qDebug() << reply->error();
        Work::m1.unlock();
    }

    //    Robot::MW->m3.lock();
    //    append(ur,Robot::MW->sl);
    //    Robot::MW->m3.unlock();

    reply->deleteLater();

    nach();
}

void Work::setStop()
{
    stop = true;
    Mythread->exit();
    //Mythread->wait();
}

void Work::funProw(QNetworkReply* reply)
{
    if (!prow) {
        prow = true;
        if (seek) {
            if (reply->hasRawHeader("Content-Range")) {
                QString s1 = reply->rawHeader("Content-Range");
                seek = s1.remove("bytes ").split("-").at(0).toInt();
            } else {
                if (reply->hasRawHeader("Content-Length")) {
                    QString s1 = reply->rawHeader("Content-Length");
                    if (seek == s1.toInt()) {
                        prop = true;
                        reply->abort();
                    } else {
                        seek = 0;
                    }
                } else {
                    prop = true;
                    reply->abort();
                }
            }
        }
    }
}

void Work::funZap(QByteArray& content)
{
    QDir d;
    QFile f(rob->dir_web + "" + put + fileName);
    bool b = d.mkpath(rob->dir_web + put);
    if (!b) {
        qDebug() << put;
    }
    if (dozap) {
        if (f.open(QIODevice::Append)) {
            f.write(content);
            f.close();
        }
    } else {
        if (f.open(QIODevice::ReadWrite)) {
            f.seek(seek);
            f.write(content);
            f.close();
        }
    }
    QString s = f.fileName();
    int i = s.lastIndexOf(".");
    if (i > 0) {
        s.remove(i, s.size());
    }
    f.setFileName(s + ".txt");
    if (f.open(QIODevice::Append)) {
        f.write(S.toUtf8());
        f.close();
    }
}

void Work::downloadProgress()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    if (stop) {
        Sl.clear();
        return;
    }
    BytesReceived = reply->size();
    funProw(reply);
    if (BytesReceived > 5000000) {
        QByteArray content = reply->readAll();
        funZap(content);
        dozap = true;
    }
}
