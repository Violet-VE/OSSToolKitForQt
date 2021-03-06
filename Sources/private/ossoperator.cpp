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
    /* ?????????OSS????????????*/
    QString AccessKeyId = accessKeyId;
    QString AccessKeySecret = accessKeySecret;
    QString Endpoint = endpoint;

    /* ???????????????????????? */
    InitializeSdk();

    client = OssClient(Endpoint.toStdString(), AccessKeyId.toStdString(), AccessKeySecret.toStdString(), conf);



    UserQLogs::saveToLog(QString::fromLatin1("??????????????????: Endpoint:")+Endpoint+" AccessKeyId:"+AccessKeyId+" AccessKeySecret:"+AccessKeySecret);
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
    UserQLogs::saveToLog(QString::fromLatin1("??????????????????."));
    UserQLogs::saveToLog(QString::fromLatin1(" ??????:")+BucketName
                         +QString::fromLatin1(" ????????????:")+Prefix);
    /* ???????????? */
    QStringList qs;
    ListObjectOutcome outcome;
    do {
        /* ???????????? */
        ListObjectsRequest request(BucketName.toStdString());

        /* ??????????????????/??????????????????????????? */
        request.setDelimiter("/");
        request.setPrefix(Prefix.toStdString());
        request.setMarker(nextMarker.toStdString());
        outcome = client.ListObjects(request);

        if (!outcome.isSuccess()) {
            /* ????????????  */
            QString errorMsg="ListObjects fail,code:"+QString::fromStdString(outcome.error().Code())
                            +",message:"+QString::fromStdString(outcome.error().Message())
                            +",requestId:"+QString::fromStdString(outcome.error().RequestId());
            UserQLogs::saveToLog(errorMsg);

            qDebug()<<errorMsg;

            qs=QStringList();
            break;
        }
        UserQLogs::saveToLog(QString::fromLatin1("????????????!"));
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

    UserQLogs::saveToLog(QString::fromLatin1("????????????OSS???????????????."));
    UserQLogs::saveToLog(QString::fromLatin1("??????:")+BucketName
                         +QString::fromLatin1(" ????????????:")+Prefix
                         +QString::fromLatin1(" ???????????????????????????:")+MetaType);

    /* ???????????? */
    QStringList qstr=listOSSObj(Prefix,BucketName);
    QStringList qs;

    for (QString object : qstr) {

        /* ?????????????????????????????? */
        auto outcome2 = client.HeadObject(BucketName.toStdString(), object.toStdString());

        if (!outcome2.isSuccess()) {
            /* ???????????? */
            QString errorMsg="HeadObject fail,code:"+QString::fromStdString(outcome2.error().Code())
                            +",message:"+QString::fromStdString(outcome2.error().Message())
                            +",requestId:"+QString::fromStdString(outcome2.error().RequestId());
            UserQLogs::saveToLog(errorMsg);

            qDebug()<<errorMsg;
            qs=QStringList();
        }
        else {
            //UserQLogs::saveToLog(QString::fromLatin1("?????? ")+QString::fromStdString(object.toStdString())+QString::fromLatin1(" ???????????????!"));
            auto headMeta = outcome2.result();
            if(MetaType=="?????????+ETag+Size"){
                qs.append(QString::fromStdString(object.toStdString()).replace(Prefix,"")+" | "+QString::fromStdString(headMeta.ETag()).toLower()+" | "+QString::number(headMeta.ContentLength()));
            }else if (MetaType=="?????????+Size"){
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
    //QSettings?????????????????????????????????ini???????????????,???????????????????????????ini??????,???????????????????????????
    QSettings *IniWrite = new QSettings(FilePath, QSettings::IniFormat);
    IniWrite->setValue(keyStr, valueStr);
    //???????????????????????????
    delete IniWrite;
}

QString OSSOperator::readConfig(QString FilePath, QString keyStr)
{
    //QSettings?????????????????????????????????ini???????????????,???????????????????????????ini??????,???????????????????????????
    QSettings *iniRead = new QSettings(FilePath, QSettings::IniFormat);
    QString ResultStr = iniRead->value(keyStr).toString();
    //???????????????????????????
    delete iniRead;
    return ResultStr;
}
