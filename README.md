# OSSToolKitForQt

## 打包代码

 假设qml资源位于`E:\Projects\QT\OSSToolKitForQt\Resources`下,且exe位于区别于项目的其他目录下

 ``` bash
  wwindeployqt --no-opengl-sw --no-angle --no-webkit2 --no-virtualkeyboard --no-translations --no-bluetooth --no-concurrent --no-declarative --no-designer --no-designercomponents --no-enginio --no-gamepad --no-qthelp --no-multimedia --no-multimediawidgets --no-multimediaquick --no-nfc --no-opengl --no-positioning --no-printsupport --no-quickparticles --no-script --no-scripttools --no-sensors --no-serialport --no-sql --no-svg --no-test --no-webkit --no-webkitwidgets --no-websockets --no-winextras --no-xml --no-xmlpatterns --no-webenginecore --no-webengine --no-webenginewidgets --no-3dcore --no-3drenderer --no-3dquick --no-3dquickrenderer --no-3dinput --no-3danimation --no-3dextras --no-geoservices --no-webchannel --no-texttospeech --no-serialbus --no-webview OSSToolKitForQt.exe --qmldir E:\Projects\QT\OSSToolKitForQt\Resources
 ```
