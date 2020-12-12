#ifndef STRINGLISTTHREAD_H
#define STRINGLISTTHREAD_H

#include <QMutex>
#include <QStringList>
#include <QTextStream>

class StringListThread {
public:
    void add(QString s);
    void append_not_mutex(QString&& s);
    void add_first(QString s);
    void clear();
    void save_to_ts(QTextStream& ts);
    int size_not_mutex();
    std::tuple<bool, QString> get_empty_size_string();

private:
    QStringList sl;
    mutable QMutex mutex;
};

#endif // STRINGLISTTHREAD_H
