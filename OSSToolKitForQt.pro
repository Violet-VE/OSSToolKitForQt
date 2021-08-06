QT += quick quickcontrols2

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Sources/main.cpp \
        Sources/private/ossoperator.cpp \
        Sources/private/userqlogs.cpp

RESOURCES += Resources/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Sources/public/ossoperator.h \
    Sources/public/userqlogs.h



INCLUDEPATH += $$PWD/third_party/include/
PRE_TARGETDEPS += $$PWD/third_party/lib/libalibabacloud-oss-cpp-sdk.a


win32: LIBS += -L$$PWD/third_party/lib/ -llibalibabacloud-oss-cpp-sdk

win32: LIBS += -L$$PWD/third_party/lib/x64 -llibcurl
win32: LIBS += -L$$PWD/third_party/lib/x64 -llibeay32
win32: LIBS += -L$$PWD/third_party/lib/x64 -lssleay32
