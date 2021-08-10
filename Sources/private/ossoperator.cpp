#include "..\public\ossoperator.h"
#include <QCoreApplication>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QDebug>
#include "..\public\userqlogs.h"
#include "alibabacloud/oss/OssClient.h"
using namespace AlibabaCloud::OSS;

OSSOperator::OSSOperator(QObject* _root,QObject *parent) : QObject(parent){
    root=_root;
}

OSSOperator::~OSSOperator()
{
    shutdownOSS();
    QString iniPath=QCoreApplication::applicationDirPath()+"/Config.ini";
    QVariant AccessKeyId,AccessKeySecret,Endpoint,BucketName,OSSPath,LocalPath;

    auto AccessKeyIdInput = root->findChild<QObject *>("accessKeyIdInput");
    auto AccessKeySecretInput = root->findChild<QObject *>("accessKeySecretInput");
    auto EndpointInput = root->findChild<QObject *>("endpointInput");
    auto BucketNameInput = root->findChild<QObject *>("bucketNameInput");
    auto OSSPathInput = root->findChild<QObject *>("netInput");
    auto LocalPathInput = root->findChild<QObject *>("localInput");

    QMetaObject::invokeMethod(AccessKeyIdInput, "getText", Q_RETURN_ARG(QVariant, AccessKeyId));
    QMetaObject::invokeMethod(AccessKeySecretInput, "getText", Q_RETURN_ARG(QVariant, AccessKeySecret));
    QMetaObject::invokeMethod(EndpointInput, "getText", Q_RETURN_ARG(QVariant, Endpoint));
    QMetaObject::invokeMethod(BucketNameInput, "getText", Q_RETURN_ARG(QVariant, BucketName));
    QMetaObject::invokeMethod(OSSPathInput, "getText", Q_RETURN_ARG(QVariant, OSSPath));
    QMetaObject::invokeMethod(LocalPathInput, "getText", Q_RETURN_ARG(QVariant, LocalPath));

    if(AccessKeyId!="")OSSOperator::saveConfig(iniPath,"Settings/AccessKeyId",AccessKeyId.toString());
    if(AccessKeySecret!="")OSSOperator::saveConfig(iniPath,"Settings/AccessKeySecret",AccessKeySecret.toString());
    if(Endpoint!="")OSSOperator::saveConfig(iniPath,"Settings/Endpoint",Endpoint.toString());
    if(BucketName!="")OSSOperator::saveConfig(iniPath,"Settings/BucketName",BucketName.toString());
    if(OSSPath!="")OSSOperator::saveConfig(iniPath,"Settings/OSSPath",OSSPath.toString());
    if(LocalPath!="")OSSOperator::saveConfig(iniPath,"Settings/LocalPath",LocalPath.toString());
}

ClientConfiguration OSSOperator::conf;
OssClient OSSOperator::client("","","",conf);

void OSSOperator::initOSS(QString accessKeyId, QString accessKeySecret, QString endpoint)
{
    /* 初始化OSS账号信息*/
    QString AccessKeyId = accessKeyId;
    QString AccessKeySecret = accessKeySecret;
    QString Endpoint = endpoint;

    /* 初始化网络等资源 */
    InitializeSdk();

    client = OssClient(Endpoint.toStdString(), AccessKeyId.toStdString(), AccessKeySecret.toStdString(), conf);



    UserQLogs::saveToLog(QString::fromLatin1("初始化完的值: Endpoint:")+Endpoint+" AccessKeyId:"+AccessKeyId+" AccessKeySecret:"+AccessKeySecret);
}

QStringList OSSOperator::uploadToOSS(QString netPath, QString bucketName)
{
    QString BucketName = bucketName;
    return QStringList();
}

QStringList OSSOperator::downloadOSS(QString netPath, QString bucketName)
{
    QString BucketName = bucketName;
    return QStringList();
}

QStringList OSSOperator::listOSSObj(QString netPath, QString bucketName)
{
    QString BucketName = bucketName;
    QString Prefix = netPath;
    QString nextMarker="";
    UserQLogs::saveToLog(QString::fromLatin1("开始列举文件."));
    UserQLogs::saveToLog(QString::fromLatin1(" 桶名:")+BucketName
                         +QString::fromLatin1(" 路径前缀:")+Prefix);
    /* 列举文件 */
    QStringList qs;
    ListObjectOutcome outcome;
    do {
        /* 列举文件 */
        ListObjectsRequest request(BucketName.toStdString());

        /* 设置正斜线（/）为文件夹的分隔符 */
        request.setDelimiter("/");
        request.setPrefix(Prefix.toStdString());
        request.setMarker(nextMarker.toStdString());
        outcome = client.ListObjects(request);

        if (!outcome.isSuccess()) {
            /* 异常处理  */
            QString errorMsg="ListObjects fail,code:"+QString::fromStdString(outcome.error().Code())
                            +",message:"+QString::fromStdString(outcome.error().Message())
                            +",requestId:"+QString::fromStdString(outcome.error().RequestId());
            UserQLogs::saveToLog(errorMsg);

            qDebug()<<errorMsg;

            qs=QStringList();
            break;
        }
        UserQLogs::saveToLog(QString::fromLatin1("列举成功!"));
        for (const auto& object : outcome.result().ObjectSummarys()) {
            qs.append(QString::fromStdString(object.Key()));
        }
        for (const auto& commonPrefix : outcome.result().CommonPrefixes()) {
            qs.append(listOSSObj(QString::fromStdString(commonPrefix),BucketName));
        }

        nextMarker = QString::fromStdString(outcome.result().NextMarker());
    } while (outcome.result().IsTruncated());

    foreach(QString s,qs){
        if (s==""||s.endsWith("/"))
            qs.removeOne(s);
    }

    return qs;
}

QStringList OSSOperator::getOSSMeta(QString netPath, QString bucketName)
{
    QString BucketName = bucketName;
    QString Prefix = netPath;

    auto selectInfo = root->findChild<QObject *>("selectInfo");
    QVariant QVMetaType;
    QMetaObject::invokeMethod(selectInfo, "getSelectedText", Q_RETURN_ARG(QVariant, QVMetaType));
    QString MetaType=QVMetaType.toString();

    UserQLogs::saveToLog(QString::fromLatin1("开始获取OSS文件元信息."));
    UserQLogs::saveToLog(QString::fromLatin1("桶名:")+BucketName
                         +QString::fromLatin1(" 路径前缀:")+Prefix
                         +QString::fromLatin1(" 需要获取的元信息为:")+MetaType);

    /* 列举文件 */
    QStringList qstr=listOSSObj(Prefix,BucketName);
    QStringList qs;

    for (QString object : qstr) {

        /* 获取文件的全部元信息 */
        auto outcome2 = client.HeadObject(BucketName.toStdString(), object.toStdString());

        if (!outcome2.isSuccess()) {
            /* 异常处理 */
            QString errorMsg="HeadObject fail,code:"+QString::fromStdString(outcome2.error().Code())
                            +",message:"+QString::fromStdString(outcome2.error().Message())
                            +",requestId:"+QString::fromStdString(outcome2.error().RequestId());
            UserQLogs::saveToLog(errorMsg);

            qDebug()<<errorMsg;
            qs=QStringList();
        }
        else {
            //UserQLogs::saveToLog(QString::fromLatin1("获取 ")+QString::fromStdString(object.toStdString())+QString::fromLatin1(" 元信息成功!"));
            auto headMeta = outcome2.result();
            if(MetaType=="文件名+ETag+Size"){
                qs.append(QString::fromStdString(object.toStdString()).replace(Prefix,"")+" | "+QString::fromStdString(headMeta.ETag()).toLower()+" | "+QString::number(headMeta.ContentLength()));
            }else if (MetaType=="文件名+Size"){
                qs.append(QString::fromStdString(object.toStdString()).replace(Prefix,"")+" | "+QString::number(headMeta.ContentLength()));
            }
        }
    }

    return qs;
}

void OSSOperator::shutdownOSS()
{
    ShutdownSdk();
}

void OSSOperator::saveToFile(QString data, QString FilePath,bool isTruncate)
{
    if(data==""||FilePath=="")return;

    QFile file(FilePath);
    if ( isTruncate ){
        file.open(QFile::ReadWrite | QFile::Truncate);
    }else{
         file.open(QFile::ReadWrite | QFile::Append);
    }
    QTextStream out(&file);
    out<<data;
    file.close();
}

void OSSOperator::clearFileContent(QString FilePath)
{
    if(FilePath=="")return;

    QFile file(FilePath);
    if ( file.exists() ) {
        if ( file.open(QFile::ReadWrite | QFile::Truncate) ){
            QTextStream out(&file);
            out<<"";
            file.close();
        }
    }
}

void OSSOperator::saveConfig(QString FilePath, QString keyStr, QString valueStr)
{
    //QSettings构造函数的第一个参数是ini文件的路径,第二个参数表示针对ini文件,第三个参数可以缺省
    QSettings *IniWrite = new QSettings(FilePath, QSettings::IniFormat);
    IniWrite->setValue(keyStr, valueStr);
    //写入完成后删除指针
    delete IniWrite;
}

QString OSSOperator::readConfig(QString FilePath, QString keyStr)
{
    //QSettings构造函数的第一个参数是ini文件的路径,第二个参数表示针对ini文件,第三个参数可以缺省
    QSettings *iniRead = new QSettings(FilePath, QSettings::IniFormat);
    QString ResultStr = iniRead->value(keyStr).toString();
    //读入完成后删除指针
    delete iniRead;
    return ResultStr;
}
