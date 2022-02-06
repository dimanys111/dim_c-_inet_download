#include "stringlistthread.h"

void StringListThread::append(const QString& s)
{
    QMutexLocker locker(&mutex);
    sl.append(s);
    if (sl.size() > 10000000) {
        sl.removeDuplicates();
        if (sl.size() > 5000000) {
            int n = sl.size() - 5000000;
            for (int i = 0; i < n; i++) {
                sl.removeLast();
            }
        }
    }
}

void StringListThread::append_not_mutex(const QString& s)
{
    sl.append(s);
}

void StringListThread::append_first(QString s)
{
    QMutexLocker locker(&mutex);
    sl.insert(0, s);
}

void StringListThread::clear()
{
    QMutexLocker locker(&mutex);
    sl.clear();
}

void StringListThread::save_to_ts(QTextStream& ts)
{
    QMutexLocker locker(&mutex);
    sl.removeDuplicates();
    for (int n = 0; n < sl.size(); n++) {
        ts << sl.at(n) << "\r\n";
    }
}

int StringListThread::size()
{
    QMutexLocker locker(&mutex);
    return sl.size();
}

std::tuple<bool, QString> StringListThread::get_empty_size_string()
{
    QMutexLocker locker(&mutex);
    if (!sl.empty()) {
        return std::make_tuple(false, sl.takeFirst());
    } else {
        return std::make_tuple(true, "");
    }
}
