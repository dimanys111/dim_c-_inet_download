#include "spisovetka.h"
#include <QDebug>
#include <QStringList>

#include "work.h"

void inc(QString& s, QStringList& sl)
{
    QStringList sl_ = s.split("?");
    if (sl_.size() > 1) {
        s = sl_.first();
        if (s == "") {
            return;
        }
    }
    s.replace("://", "/");
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
    sl = s.split("/");
    if (sl.last() == "") {
        sl.removeLast();
    }
    sl.removeAll("");
}

void SpisoVetka::append(QString s)
{
    if (s.size() > 0 && s.indexOf("http") == 0) {
        QStringList sl;
        inc(s, sl);
        add(sl);
    }
}

bool SpisoVetka::contains(QString s)
{
    if (s.size() == 0) {
        return true;
    }
    QStringList sl;
    inc(s, sl);
    for (auto& a : SV) {
        if (a.contains(sl)) {
            return true;
        }
    }
    return false;
}

bool SpisoVetka::contains(QStringList& sl)
{
    if (item == sl.at(nom)) {
        if (sl.size() - 1 == nom) {
            if (con) {
                return true;
            } else {
                return false;
            }
        } else {
            for (auto& a : SV) {
                if (a.contains(sl)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool SpisoVetka::contains_one(QStringList& sl)
{
    if (item == sl.at(nom)) {
        return true;
    }
    return false;
}

SpisoVetka& SpisoVetka::get_sv(QStringList& sl)
{
    for (auto& a : SV) {
        if (a.contains_one(sl)) {
            return a;
        }
    }
    SV << SpisoVetka(nom + 1, sl.at(nom + 1));
    return SV.last();
}

void SpisoVetka::add(QStringList& sl)
{
    if (nom == sl.size() - 2)
        get_sv(sl).con = true;
    else if (nom < sl.size() - 2)
        get_sv(sl).add(sl);
}

void SpisoVetka::zag(QString s)
{
    QStringList sl = s.split("/");
    get_sv(sl).add(sl);
}

void SpisoVetka::save_SV(QTextStream& ts)
{
    int n = SV.size();
    for (int i = 0; i < n; i++) {
        SV.at(i).save_SV(ts, item);
    }
}

void SpisoVetka::save_SV(QTextStream& ts, QString s) const
{
    if (SV.empty()) {
        ts << s + "/" + item << "\r\n";
    } else {
        int n = SV.size();
        for (int i = 0; i < n; i++) {
            QString s1 = s + "/" + item;
            SV.at(i).save_SV(ts, s1);
        }
    }
}

SpisoVetka::~SpisoVetka()
{
    clear();
}

void SpisoVetka::clear()
{
    SV.clear();
}

int SpisoVetka::size() const
{
    int n = 0;
    n = SV.size();
    int z = SV.size();
    for (int i = 0; i < z; i++) {
        n = n + SV.at(i).size();
    }
    return n;
}
