#include "..\public\ossoperator.h"
#include <QCoreApplication>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QSettings>
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

    OSSOperator::saveConfig(iniPath,"Settings/AccessKeyId",AccessKeyId.toString());
    OSSOperator::saveConfig(iniPath,"Settings/AccessKeySecret",AccessKeySecret.toString());
    OSSOperator::saveConfig(iniPath,"Settings/Endpoint",Endpoint.toString());
    OSSOperator::saveConfig(iniPath,"Settings/BucketName",BucketName.toString());
    OSSOperator::saveConfig(iniPath,"Settings/OSSPath",OSSPath.toString());
    OSSOperator::saveConfig(iniPath,"Settings/LocalPath",LocalPath.toString());
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
    QString marker = netPath;
    /* 列举文件 */
    QStringList qs;
    ListObjectOutcome outcome;
    do {
        /* 列举文件 */
        ListObjectsRequest request(BucketName.toStdString());
        /* 列举指定marker之后的文件 */
        request.setPrefix(marker.toStdString());
        outcome = client.ListObjects(request);

        if (!outcome.isSuccess()) {
            /* 异常处理  */
            std::cout << "ListObjects fail" <<
            ",code:" << outcome.error().Code() <<
            ",message:" << outcome.error().Message() <<
            ",requestId:" << outcome.error().RequestId() << std::endl;
            qs=QStringList();
            break;
        }
        for (const auto& object : outcome.result().ObjectSummarys()) {
            qs.append(QString::fromStdString(object.Key()));
        }
    } while (outcome.result().IsTruncated());
    return qs;
}

QStringList OSSOperator::getOSSMeta(QString netPath, QString bucketName)
{
    QString BucketName = bucketName;
    QString marker = netPath;

    auto selectInfo = root->findChild<QObject *>("selectInfo");
    QVariant QVMetaType;
    QMetaObject::invokeMethod(selectInfo, "getSelectedText", Q_RETURN_ARG(QVariant, QVMetaType));
    QString MetaType=QVMetaType.toString();

    /* 列举文件 */
    QStringList qs;
    ListObjectOutcome outcome;
    do {
        /* 列举文件 */
        ListObjectsRequest request(BucketName.toStdString());
        /* 列举指定marker之后的文件 */
        request.setPrefix(marker.toStdString());
        outcome = client.ListObjects(request);

        if (!outcome.isSuccess()) {
            /* 异常处理  */
            std::cout << "ListObjects fail" <<
            ",code:" << outcome.error().Code() <<
            ",message:" << outcome.error().Message() <<
            ",requestId:" << outcome.error().RequestId() << std::endl;
            qs=QStringList();
            break;
        }
        for (const auto& object : outcome.result().ObjectSummarys()) {

            /* 获取文件的全部元信息 */
            auto outcome2 = client.HeadObject(BucketName.toStdString(), object.Key());

            if (!outcome.isSuccess()) {
                /* 异常处理 */
                std::cout << "HeadObject fail" <<
                ",code:" << outcome.error().Code() <<
                ",message:" << outcome.error().Message() <<
                ",requestId:" << outcome.error().RequestId() << std::endl;
                ShutdownSdk();
                return QStringList();
            }
            else {
                auto headMeta = outcome2.result();
                if(MetaType=="文件名+ETag"){
                    qs.append(QString::fromStdString(object.Key())+" | "+QString::fromStdString(headMeta.ETag()));
                }else if (MetaType=="单文件名"){
                    qs.append(QString::fromStdString(object.Key()));
                }
            }
        }
    } while (outcome.result().IsTruncated());

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
