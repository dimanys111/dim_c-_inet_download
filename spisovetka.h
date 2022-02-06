#ifndef SPISOVETKA_H
#define SPISOVETKA_H

#include <QHash>
#include <QString>
#include <QStringList>
#include <QTextStream>

class SpisoVetka {
public:
    SpisoVetka(int nom, const QString& item)
        : nom(nom)
        , item(item)
    {
    }
    bool contains(QString s);
    void zag(QString s);
    void append(QString s);
    void save_SV(QTextStream& ts);
    void clear();
    int size() const;
    ~SpisoVetka();

private:
    int nom { 0 };
    QList<SpisoVetka> SV;
    QString item;
    bool con { false };
    bool contains(QStringList& sl);
    void add(QStringList& sl);
    void save_SV(QTextStream& ts, QString s) const;
    SpisoVetka& get_sv(QStringList& sl);
    bool contains_one(QStringList& sl);
};

#endif // SPISOVETKA_H
