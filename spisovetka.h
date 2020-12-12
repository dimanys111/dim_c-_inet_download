#ifndef SPISOVETKA_H
#define SPISOVETKA_H

#include <QHash>
#include <QString>
#include <QStringList>
#include <QTextStream>

class SpisoVetka {
public:
    bool contains(QString s);
    void zag(QString s);
    void append(QString s);
    void save_SV(QTextStream& ts);
    void clear();
    int size();
    ~SpisoVetka();

private:
    QList<SpisoVetka*> SV;
    QList<bool> boo;
    QList<char*> list;
    QList<char*> listProm;
    bool contains(QStringList& sl, int n);
    void add_zag(QStringList& sl, int n);
    void add(QStringList& sl, int n);
    void save_SV(QTextStream& ts, QString s);
};

#endif // SPISOVETKA_H
