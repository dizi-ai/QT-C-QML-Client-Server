import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import Backend 1.0

Window {
    visible: true
    width: 700
    minimumWidth: 500
    height: 450
    minimumHeight: 200
    title: "Client"
    color: "#CED0D4"

    Backend {
        id: backend

        onStatusChanged: {
            ti.append(addMsg(newStatus));
            if (currentStatus !== true)
            {
                btn_connect.enabled = true;
            }
        }
        onSomeMessage: {
            ti.append(addMsg(msg));
        }
        onSomeError: {
            ti.append(addMsg("Error! " + err));
            if (currentStatus !== true)
            {
                backend.disconnectClicked();
            }
            btn_connect.enabled = true;
        }
        onSomeStatistic: {
            ti2.append(msg)
        }
    }
    FileDialog {
        id: filedialog
        folder: "."
        title: "Выберите файл"
        selectMultiple: false
        nameFilters: [ "Text files (*.txt)", "All files (*)" ]
        onAccepted: {
            var path = filedialog.fileUrl.toString();
            path = path.replace(/^(file:\/{3})/,"");
            var cleanPath = decodeURIComponent(path);
            backend.setFilePath(cleanPath)
        }
    }

    ColumnLayout{
        anchors.fill: parent
        anchors.margins: 10

        LayoutSection {
            height: status.height + 20
            color: backend.currentStatus ? "#CAF5CF" : "#EA9FA9"

            Text {
                id: status
                anchors.centerIn: parent
                text: backend.currentStatus ? "УСТНОВЛЕНО" : "НЕ УСТАНОВЛЕНО"
                font.weight: Font.Bold
            }
            BetterButton {
                id: btn_connect
                anchors.centerIn: parent.Center
                height: parent.height
                text: "Подключиться к серверу"
                color: enabled ? this.down ? "#78C37F" : "#87DB8D" : "gray"
                border.color: "#78C37F"
                onClicked: {
                    ti.append(addMsg("Подключение к серверу..."));
                    backend.connectClicked();
                    this.enabled = false;
                }
            }
            BetterButton {
                id: btn_disconnect
                enabled: !btn_connect.enabled
                anchors.right: parent.right
                height: parent.height
                text: "Отключиться от сервера"
                color: enabled ? this.down ? "#DB7A74" : "#FF7E79" : "gray"
                border.color: "#DB7A74"
                onClicked: {
                    ti.append(addMsg("Отключение от сервера..."));
                    backend.disconnectClicked();
                    btn_connect.enabled = true;
                }
            }
        }

        LayoutSection {
            anchors.horizontalCenter: parent.horizontalCenter
            height: btn_stats.height
            ComboBox{
                id: cb_statistics
                anchors.left: parent.left
                height: parent.height
                width: parent.width*2/3
                model: ["История сообщений", "Статистика по длине слов", "Статистика по вхождению символов", "Статистика запросов от пользователя"]
            }
            Button{
                id: btn_stats
                text: "Показать"
                anchors.right: parent.right
                width: parent.width*1/3
                onClicked: {
                    if (cb_statistics.currentValue == "История сообщений"){
                        scrollView.visible = true
                        scrollView2.visible = false
                    }
                    else if (cb_statistics.currentValue == "Статистика по длине слов"){
                        scrollView.visible = false
                        ti2.clear()
                        backend.getWordsLen()
                        scrollView2.visible = true
                    }
                    else if (cb_statistics.currentValue == "Статистика по вхождению символов"){
                        scrollView.visible = false
                        ti2.clear()
                        backend.getCharCount()
                        scrollView2.visible = true
                    }
                    else if (cb_statistics.currentValue == "Статистика запросов от пользователя"){
                        scrollView.visible = false
                        ti2.clear()
                        backend.getRequests()
                        scrollView2.visible = true
                    }
                }
            }
        }

        LayoutSection {
            id: layout_history
            Layout.fillHeight: true

            ScrollView {
                id: scrollView
                anchors.fill: parent
                TextArea {
                    id: ti
                    height: parent.height
                    readOnly: true
                    selectByMouse : true
                    font.pixelSize: 14
                    wrapMode: TextInput.WrapAnywhere
                }
            }
            ScrollView {
                id: scrollView2
                anchors.fill: parent
                TextArea {
                    id: ti2
                    height: parent.height
                    readOnly: true
                    selectByMouse : true
                    font.pixelSize: 14
                    wrapMode: TextInput.WrapAnywhere
                }
            }
        }

        RowLayout {
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            Layout.topMargin: 0
            Layout.bottomMargin: 5
            Rectangle {
                id : upper2
                height: fileload.height
                Layout.fillWidth: true
                ComboBox{
                    id:cb_request
                    height: fileload.height
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width*1/2
                    model: ["Количество вхождений каждого символа", "Распределение слов по их длинам", "Отправленные запросы"]
                }
                BetterButton {
                    id : fileload
                    x: cb_request.width
                    color: "#d7d9db"
                    text: "Выбрать файл"
                    width: parent.width*1/4
                    onClicked: {
                        filedialog.open()
                    }

                }
                BetterButton {
                    id: btn_send
                    x: fileload.width+cb_request.width
                    height: fileload.height
                    width: parent.width*1/4
                    enabled: !btn_connect.enabled
                    text: "Отправить"
                    color: enabled ? this.down ? "#6FA3D2" : "#7DB7E9" : "gray"
                    border.color: "#6FA3D2"
                    onClicked: {
                        if (cb_request.currentValue == "Отправленные запросы"){
                            backend.sendClicked(2)
                            ti.append(addMsg("Отправка запроса..."));
                        }
                        else if (filedialog.fileUrl == "") {
                            ti.append(addMsg("Не выбран файл..."));
                        }
                        else {
                            ti.append(addMsg("Отправка запроса..."));
                            if (cb_request.currentValue == "Количество повторений каждого символа") {
                                backend.sendClicked(0)
                            }
                            else if (cb_request.currentValue =="Распределение слов по их длинам"){
                                backend.sendClicked(1)
                            }
                        }
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        ti.text = addMsg("Начало работы\n- - - - - -", false);
    }

    function addMsg(someText)
    {
        return "[" + currentTime() + "] " + someText;
    }

    function currentTime()
    {
        var now = new Date();
        var nowString = ("0" + now.getHours()).slice(-2) + ":"
                + ("0" + now.getMinutes()).slice(-2) + ":"
                + ("0" + now.getSeconds()).slice(-2);
        return nowString;
    }
}
