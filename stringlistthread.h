#ifndef STRINGLISTTHREAD_H
#define STRINGLISTTHREAD_H

#include <QMutex>
#include <QStringList>
#include <QTextStream>

class StringListThread {
public:
    void append_not_mutex(const QString& s);
    void append_first(QString s);
    void clear();
    void save_to_ts(QTextStream& ts);
    int size();
    std::tuple<bool, QString> get_empty_size_string();
    void append(const QString& s);

private:
    QStringList sl;
    mutable QMutex mutex;
};

#endif // STRINGLISTTHREAD_H
