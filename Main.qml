import QtQuick 
import QtQuick.Controls 

import blockchain

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

            Connections{
                target: BlockchainApp

                function onPrintMsg(message){
                    textArea.text = message
                }

            }

            Button{
                id: printBlock
                anchors{
                    left: textArea.left
                    top: textArea.bottom
                    topMargin: 5
                }
                height: 30
                text: "Print Block"

                onClicked: {
                    if(blockIndex.text != ""){
                        BlockchainApp.getBlockInfo(parseInt(blockIndex.text))
                    }
                }

            }

            TextField{
                id: blockIndex
                anchors{
                    left: printBlock.right
                    leftMargin: 10
                    verticalCenter: printBlock.verticalCenter
                }

                horizontalAlignment: TextField.AlignHCenter
                width: 40
            }


        }
    }
}
