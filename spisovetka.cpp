#include "spisovetka.h"
#include <QDebug>
#include <QStringList>

#include "work.h"

void inc(QString& s, QStringList& sl, int& n)
{
    QStringList sl_ = s.split("?");
    if (sl_.size() > 1) {
        s = sl_.first();
        if (s == "") {
            return;
        }
    }
    s.remove("http://").remove("https://");
    s.replace(";", "/");
    s.replace("=", "/");
    s.replace("-", "/");
    s.replace("&", "/");
    s.replace(".", "/");
    s.replace(",", "/");
    s.replace(" ", "/");
    s.replace("_", "/");
    if (s == "") {
        return;
    }
    if (s.at(s.size() - 1) == '/')
        s.remove(s.size() - 1, 1);
    n = 0;
    sl = s.split("/");
    sl.removeAll("");
}

void SpisoVetka::append(QString s)
{
    if (s.size() > 0 && s.indexOf("http") == 0) {
        int n;
        QStringList sl;
        inc(s, sl, n);
        add(sl, n);
    }
}

bool SpisoVetka::contains(QString s)
{
    if (s.size() == 0) {
        return true;
    }
    int n;
    QStringList sl;
    inc(s, sl, n);
    return contains(sl, n);
}

bool SpisoVetka::contains(QStringList& sl, int n)
{
    if (sl.size() > n + 1) {
        n++;
        int i = containsListChar(listProm, sl.at(n - 1));
        if (i > -1) {
            SpisoVetka* ssvv = SV.at(i);
            return ssvv->contains(sl, n);
        } else {
            listProm << StrToChar(sl.at(n - 1));
            SpisoVetka* ssvv = new SpisoVetka();
            SV << ssvv;
            ssvv->add_zag(sl, n);
            return false;
        }
    } else {
        if (sl.size() > n) {
            int i = containsListChar(listProm, sl.at(n));
            if (i > -1)
                return true;
            i = containsListChar(list, sl.at(n));
            if (i > -1)
                return true;
            else {
                list << StrToChar(sl.at(n));
                boo << false;
                return false;
            }
        }
    }
    return false;
}

void SpisoVetka::zag(QString s)
{
    int n = 0;
    QStringList sl = s.split("/");
    add_zag(sl, n);
}

void SpisoVetka::add_zag(QStringList& sl, int n)
{
    if (sl.size() > n + 1) {
        n++;
        SpisoVetka* ssvv;
        int i = containsListChar(listProm, sl.at(n - 1));
        if (i > -1)
            ssvv = SV.at(i);
        else {
            listProm << StrToChar(sl.at(n - 1));
            ssvv = new SpisoVetka();
            SV << ssvv;
        }
        ssvv->add_zag(sl, n);
    } else {
        if (sl.size() > n) {
            list << StrToChar(sl.at(n));
            boo << false;
        }
    }
}

void SpisoVetka::add(QStringList& sl, int n)
{
    if (sl.size() > n + 1) {
        n++;
        SpisoVetka* ssvv;
        int i = containsListChar(listProm, sl.at(n - 1));
        if (i > -1)
            ssvv = SV.at(i);
        else {
            listProm << StrToChar(sl.at(n - 1));
            ssvv = new SpisoVetka();
            SV << ssvv;
        }
        ssvv->add(sl, n);
    } else {
        if (sl.size() > n) {
            int i = containsListChar(list, sl.at(n));
            //            if (i==-1)
            //            {
            //                sv.list<<StrToChar(sl.at(n));
            //            }
            if (i > -1) {
                boo.replace(i, true);
            }
        }
    }
}

void SpisoVetka::save_SV(QTextStream& ts)
{
    int n = SV.size();
    for (int i = 0; i < n; i++) {
        QString s = QString().fromUtf8(listProm.at(i));
        SV.at(i)->save_SV(ts, s);
    }
    n = list.size();
    for (int i = 0; i < n; i++) {
        if (boo.at(i)) {
            ts << QString().fromUtf8(list.at(i)) << "\r\n";
        }
    }
}

void SpisoVetka::save_SV(QTextStream& ts, QString s)
{
    int n = SV.size();
    for (int i = 0; i < n; i++) {
        QString s1 = s + "/" + QString().fromUtf8(listProm.at(i));
        SV.at(i)->save_SV(ts, s1);
    }
    n = list.size();
    for (int i = 0; i < n; i++) {
        if (boo.at(i)) {
            QString s1 = s + "/" + QString().fromUtf8(list.at(i));
            ts << s1 << "\r\n";
        }
    }
}

SpisoVetka::~SpisoVetka()
{
    clear();
}

void SpisoVetka::clear()
{
    int n = SV.size();
    for (int i = 0; i < n; i++) {
        delete SV.at(i);
    }
    SV.clear();
    n = list.size();
    for (int i = 0; i < n; i++) {
        delete[] list.at(i);
    }
    list.clear();
    boo.clear();
    n = listProm.size();
    for (int i = 0; i < n; i++) {
        delete[] listProm.at(i);
    }
    listProm.clear();
}

int SpisoVetka::size()
{
    int n = 0;
    n = list.size();
    int z = SV.size();
    for (int i = 0; i < z; i++) {
        n = n + SV.at(i)->size();
    }
    return n;
}
