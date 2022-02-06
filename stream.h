#ifndef STREAM_H
#define STREAM_H

#include <QImage>
#include <QObject>

class Stream : public QObject {
    Q_OBJECT

public:
    Stream();
    ~Stream();
public slots:
    void start_slot(QString in_f, QString name_mod);
    void end_slot();
signals:
    void emit_out_file(QString s);
};

#endif // STREAM_H
