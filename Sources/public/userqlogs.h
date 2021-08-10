#ifndef USERQLOGS_H
#define USERQLOGS_H

#include <QObject>

class UserQLogs : public QObject
{
    Q_OBJECT
public:
    explicit UserQLogs(QObject *parent = nullptr);
    static QString GetCurrentTime();
    static QString GetCurrentDate();
    static bool saveToLog(QString data);

signals:

};

#endif // USERQLOGS_H
