import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1
import QtQuick.Dialogs 1.2

Window {
    id:root
    width: Screen.width/2
    height: 300
    minimumWidth: 500
    minimumHeight: 300
    visible: true
    title: qsTr("文件MD5计算")

    modality: Qt.WindowModal


    Column{
        id: topBox
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: bottomBox.top
        spacing:10
        padding: 10

        Row{
            height: (parent.height-parent.spacing*3)/3
            width: parent.width
            spacing:10
            Label{
                id: accessKeyIdLabel
                text: qsTr("AccessKeyId:")
                height: parent.height
                verticalAlignment: "AlignVCenter"
            }
            TextField{
                objectName: "accessKeyIdInput"
                id: accessKeyIdInput
                height: parent.height
                width: (parent.width-accessKeyIdLabel.width-accessKeySecretLabel.width-parent.spacing*3-20)/2
                selectByMouse: true

                function getText(){
                    return accessKeyIdInput.text;
                }

                function setText(t){
                    accessKeyIdInput.text = t
                }
            }
            Label{
                id: accessKeySecretLabel
                text: qsTr("AccessKeySecret:")
                height: parent.height
                verticalAlignment: "AlignVCenter"
            }
            TextField{
                objectName: "accessKeySecretInput"
                id: accessKeySecretInput
                height: parent.height
                width: (parent.width-accessKeyIdLabel.width-accessKeySecretLabel.width-parent.spacing*3-20)/2
                selectByMouse: true

                function getText(){
                    return accessKeySecretInput.text;
                }

                function setText(t){
                    accessKeySecretInput.text = t
                }
            }
        }

        Row{
            height: (parent.height-parent.spacing*3)/3
            width: parent.width
            spacing:10
            Label{
                id: endpointLabel
                text: qsTr("Endpoint:")
                height: parent.height
                verticalAlignment: "AlignVCenter"
            }
            TextField{
                objectName: "endpointInput"
                id: endpointInput
                height: parent.height
                width: (parent.width-endpointLabel.width-bucketNameLabel.width-parent.spacing*3-20)/2
                selectByMouse: true

                function getText(){
                    return endpointInput.text;
                }

                function setText(t){
                    endpointInput.text = t
                }
            }
            Label{
                id: bucketNameLabel
                text: qsTr("BucketName:")
                height: parent.height
                verticalAlignment: "AlignVCenter"
            }
            TextField{
                objectName: "bucketNameInput"
                id: bucketNameInput
                height: parent.height
                width: (parent.width-endpointLabel.width-bucketNameLabel.width-parent.spacing*3-20)/2
                selectByMouse: true

                function getText(){
                    return bucketNameInput.text;
                }

                function setText(t){
                    bucketNameInput.text = t
                }
            }
        }

        Row{
            height: (parent.height-parent.spacing*3)/3
            width: parent.width
            spacing:10
            Label{
                id: netLabel
                text: qsTr("OSS路径:")
                height: parent.height
                verticalAlignment: "AlignVCenter"
            }
            TextField{
                objectName: "netInput"
                id: netInput
                height: parent.height
                width: (parent.width-netLabel.width-localLabel.width-browserLocalBtn.width-parent.spacing*4-20)/2
                selectByMouse: true

                function getText(){
                    return netInput.text;
                }

                function setText(t){
                    netInput.text = t
                }
            }
            Label{
                id: localLabel
                text: qsTr("本地路径:")
                height: parent.height
                verticalAlignment: "AlignVCenter"
            }
            TextField{
                objectName: "localInput"
                id: localInput
                height: parent.height
                width: (parent.width-netLabel.width-localLabel.width-browserLocalBtn.width-parent.spacing*4-20)/2
                text: String(selectDialog.folder).replace("file:///","")
                readOnly: true

                selectByMouse: true

                function getText(){
                    return localInput.text;
                }

                function setText(t){
                    localInput.text = t
                }
            }
            Button{
                id: browserLocalBtn
                height: parent.height
                text: qsTr("选择")
                onClicked:{
                    selectDialog.open()
                }
            }
        }


        FolderDialog {
            id: selectDialog
            title: qsTr("选择你的本地保存路径")
            folder: StandardPaths.standardLocations(StandardPaths.DesktopLocation)[0]
            options: FolderDialog.ShowDirsOnly
            onAccepted: {
                var selectedFolder = String(selectDialog.folder)
                localInput.setText(selectedFolder.replace("file:///",""))
            }
        }
    }

    MessageDialog{
        id: inputIsNullDialog
        title: "警告"
        icon: StandardIcon.Warning
        text: qsTr("输入框不能有空，请输入")
        standardButtons: StandardButton.Yes
    }

    Row{
        id: bottomBox
        anchors.bottom: parent.bottom
        height: 63
        width: parent.width
        padding: 10
        spacing: 10
        CheckBox{
            id: isOutputFile
            text: qsTr("输出结果到文件")
            height: parent.height-20
            checked: true
        }
        Button{
            id: uploadBtn
            height: parent.height-20
            text: qsTr("上传")
            width: (parent.width-isOutputFile.width-parent.spacing*5-20)/5
            enabled: false
            onClicked: {}
        }
        Button{
            id: downloadBtn
            height: parent.height-20
            text: qsTr("下载")
            width: (parent.width-isOutputFile.width-parent.spacing*5-20)/5
            enabled: false
            onClicked: {}
        }
        Button{
            id: listBtn
            height: parent.height-20
            text: qsTr("列举")
            width: (parent.width-isOutputFile.width-parent.spacing*5-20)/5
            onClicked: {
                if(netInput.text === "" &&
                        accessKeyIdInput.text === "" &&
                        accessKeySecretInput.text === "" &&
                        localInput.text === "" &&
                        endpointInput.text === "" &&
                        bucketNameInput.text === ""){
                    inputIsNullDialog.open()
                }else{
                    oss.initOSS(accessKeyIdInput.text,accessKeySecretInput.text,endpointInput.text)
                    var listResult = oss.listOSSObj(netInput.text,bucketNameInput.text)
                    if(isOutputFile.checked){
                        oss.clearFileContent(localInput.text+"/ListResult.txt")
                        for(var i=0;i<listResult.length;i++){
                            oss.saveToFile(listResult[i].replace(netInput.text,"")+"\r\n",localInput.text+"/ListResult.txt")
                        }
                    }
                }
            }
        }
        ComboBox{
            objectName: "selectInfo"
            id: selectInfo
            height: parent.height-20
            width: (parent.width-isOutputFile.width-parent.spacing*5-20)/5

            model: ListModel{
                id:model
                ListElement{text:qsTr("文件名+ETag+Size")}
                ListElement{text:qsTr("文件名+Size")}
//                ListElement{text:qsTr("最后修改时间")}
//                ListElement{text:qsTr("文件大小")}
            }

            function getSelectedText(){
                return selectInfo.displayText
            }
        }
        Button{
            id: getInfoBtn
            height: parent.height-20
            text: qsTr("获取元信息")
            width: (parent.width-isOutputFile.width-parent.spacing*5-20)/5
            onClicked: {
                if(netInput.text === "" &&
                        accessKeyIdInput.text === "" &&
                        accessKeySecretInput.text === "" &&
                        localInput.text === "" &&
                        endpointInput.text === "" &&
                        bucketNameInput.text === ""){
                    inputIsNullDialog.open()
                }else{
                    oss.initOSS(accessKeyIdInput.text,accessKeySecretInput.text,endpointInput.text)
                    var listResult = oss.getOSSMeta(netInput.text,bucketNameInput.text)
                    if(isOutputFile.checked){
                        oss.clearFileContent(localInput.text+"/ListResult.txt")
                        for(var i=0;i<listResult.length;i++){
                            oss.saveToFile(listResult[i]+"\r\n",localInput.text+"/ListResult.txt")
                        }
                    }
                }
            }
        }
    }
}
