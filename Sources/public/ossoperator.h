#ifndef OSSOPERATOR_H
#define OSSOPERATOR_H

#include <QObject>
#include "alibabacloud/oss/OssClient.h"
using namespace AlibabaCloud::OSS;

class OSSOperator : public QObject
{
    Q_OBJECT
public:
    explicit OSSOperator(QObject* root,QObject *parent = nullptr);
    ~OSSOperator();

    static ClientConfiguration conf;
    static OssClient client;

    Q_INVOKABLE void initOSS(QString AccessKeyId,QString AccessKeySecret,QString Endpoint);
    Q_INVOKABLE QStringList uploadToOSS(QString netPath,QString bucketName);
    Q_INVOKABLE QStringList downloadOSS(QString netPath,QString bucketName);
    Q_INVOKABLE QStringList listOSSObj(QString netPath,QString bucketName);
    Q_INVOKABLE QStringList getOSSMeta(QString netPath,QString bucketName);
    Q_INVOKABLE void shutdownOSS();

    Q_INVOKABLE void saveToFile(QString data,QString FilePath,bool isTruncate=false);
    Q_INVOKABLE void clearFileContent(QString FilePath);

    static void saveConfig(QString FilePath,QString keyStr,QString valueStr);
    static QString readConfig(QString FilePath,QString keyStr);

    QObject* root;

signals:

};

#endif // OSSOPERATOR_H
