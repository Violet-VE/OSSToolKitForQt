#include <QGuiApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "public/ossoperator.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);


    auto root = engine.rootObjects().first();

    OSSOperator oss(root);

    auto context = engine.rootContext();
    context->setContextProperty("oss",&oss);

    QString iniPath=QCoreApplication::applicationDirPath()+"/Config.ini";
    QString AccessKeyId=OSSOperator::readConfig(iniPath,"Settings/AccessKeyId");
    QString AccessKeySecret=OSSOperator::readConfig(iniPath,"Settings/AccessKeySecret");
    QString Endpoint=OSSOperator::readConfig(iniPath,"Settings/Endpoint");
    QString BucketName=OSSOperator::readConfig(iniPath,"Settings/BucketName");
    QString OSSPath=OSSOperator::readConfig(iniPath,"Settings/OSSPath");
    QString LocalPath=OSSOperator::readConfig(iniPath,"Settings/LocalPath");

    auto AccessKeyIdInput = root->findChild<QObject *>("accessKeyIdInput");
    auto AccessKeySecretInput = root->findChild<QObject *>("accessKeySecretInput");
    auto EndpointInput = root->findChild<QObject *>("endpointInput");
    auto BucketNameInput = root->findChild<QObject *>("bucketNameInput");
    auto OSSPathInput = root->findChild<QObject *>("netInput");
    auto LocalPathInput = root->findChild<QObject *>("localInput");

    QMetaObject::invokeMethod(AccessKeyIdInput, "setText", Q_ARG(QVariant, AccessKeyId));
    QMetaObject::invokeMethod(AccessKeySecretInput, "setText", Q_ARG(QVariant, AccessKeySecret));
    QMetaObject::invokeMethod(EndpointInput, "setText", Q_ARG(QVariant, Endpoint));
    QMetaObject::invokeMethod(BucketNameInput, "setText", Q_ARG(QVariant, BucketName));
    QMetaObject::invokeMethod(OSSPathInput, "setText", Q_ARG(QVariant, OSSPath));
    QMetaObject::invokeMethod(LocalPathInput, "setText", Q_ARG(QVariant, LocalPath));




    return app.exec();
}
