import QtQuick 
import QtQuick.Controls 

ApplicationWindow {
    visible: true
    width: 680
    height: 480
    title: "Blockchain App"

    Rectangle {
        anchors.fill: parent
        color: "#181818"

        TextArea {
            id: textArea
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }


            height: 314
            anchors.margins: 12
            color: "white"
            background: Rectangle {
                color: "black"
                border.color: "#1f1f1f"
                border.width: 2
            }
            placeholderText: "Enter text here..."
        }
    }
}
