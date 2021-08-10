#include "../public/userqlogs.h"
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>
#include <QFile>
#include <QDir>

UserQLogs::UserQLogs(QObject *parent) : QObject(parent){}

QString UserQLogs::GetCurrentTime()
{
    return QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ");
}

QString UserQLogs::GetCurrentDate()
{
    return QDateTime::currentDateTime().toString("-yyyy-MM-dd");
}

bool UserQLogs::saveToLog(QString data)
{
    if (!QFile::exists(QCoreApplication::applicationDirPath()+"/Logs")){
        QDir d;
        d.mkdir(QCoreApplication::applicationDirPath()+"/Logs");
    }
    QString LogPath=QCoreApplication::applicationDirPath()+"/Logs/Log"+GetCurrentDate()+".log";
    QFile f(LogPath);
    QString WriteData;
    if (f.open(QFile::Append | QFile::ReadWrite)){
        data+="\n";
        WriteData=GetCurrentTime()+data;
        if(f.write(data.toLatin1())!=-1){
            return true;
        }
        else return false;
    }else return false;
}
