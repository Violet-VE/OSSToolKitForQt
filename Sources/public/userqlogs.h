#ifndef USERQLOGS_H
#define USERQLOGS_H

#include <QObject>

class UserQLogs : public QObject
{
    Q_OBJECT
public:
    explicit UserQLogs(QObject *parent = nullptr);

signals:

};

#endif // USERQLOGS_H
