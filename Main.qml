pragma ComponentBehavior: Bound

import QtQuick 
import QtQuick.Controls
import QtQuick.Layouts

import blockchain

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Blockchain App"

    Rectangle {
        anchors.fill: parent
        color: "#181818"

        ScrollView{
            id: scrollView

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            
            height: 314

            TextArea {
                id: textArea
                width: parent.width
                wrapMode: Text.Wrap
                // anchors {
                //     top: parent.top
                //     left: parent.left
                //     right: parent.right
                // }


                //height: 314
                //anchors.margins: 12
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
            }
        }
        

        Button{
            id: printBlock
            anchors{
                left: scrollView.left
                top: scrollView.bottom
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
                leftMargin: 5
                verticalCenter: printBlock.verticalCenter
            }

            horizontalAlignment: TextField.AlignHCenter
            width: 40
        }

        Button{
            id: readAll
            text: "Print All Blocks"
            height: 30

            anchors{
                left: blockIndex.right
                top: scrollView.bottom
                leftMargin: 5
                topMargin: 5
            }

            onClicked: BlockchainApp.readAllBlocks();
        }

        Button{
            id: genBlock
            text: "Generate\nNew Block"

            anchors{
                left: scrollView.left
                top: printBlock.bottom
                topMargin: 5
            }

            onClicked: BlockchainApp.generateBlock()
        }

        Button {
            id: viewTransactions
            text: "View Pending\nTransactions"

            anchors{
                top: genBlock.top
                left: genBlock.right
                leftMargin: 5
            }

            onClicked: BlockchainApp.printPendingTransactions()
        }

        Item{
            id: keyDisplay
            height:50

            anchors{
                left: parent.left
                right: parent.right
                leftMargin: 10
                top: genBlock.bottom
                rightMargin: 5
            }

            RowLayout{
                anchors.fill:parent

                ColumnLayout{
                    Layout.fillHeight: true                    
                    Layout.fillWidth:true

                    RowLayout{
                        Layout.fillWidth: true
                        Text{
                            text: "Public Key"
                            Layout.preferredWidth: 60
                            horizontalAlignment: Text.AlignHCenter
                            color: "white"
                        }

                        TextField{
                            id: pubKey
                            Layout.fillWidth:true

                            Connections{
                                target: BlockchainApp
                                function onShowPubKey(key){
                                    pubKey.text = key
                                }
                            }
                        }
                    }

                    RowLayout{
                        Layout.fillWidth: true
                        Text{
                            text: "Private Key"
                            Layout.preferredWidth: 60
                            horizontalAlignment: Text.AlignHCenter
                            color: "white"
                        }

                        TextField{
                            id: privKey
                            Layout.fillWidth:true

                            Connections{
                                target: BlockchainApp
                                function onShowPrivKey(key){
                                    privKey.text = key
                                }
                            }
                        }
                    }
                }

                Button{
                    id: genWallet
                    text: "Generate\nWallet"
                    Layout.preferredHeight: 50
                    onClicked: BlockchainApp.generateWallet()
                }

                Button{
                    id: validateKeys
                    text: "Validate\nKey Pair"
                    Layout.preferredHeight: 50
                    onClicked: BlockchainApp.validateWallet(privKey.text,pubKey.text)
                }

            }
            
        }

        Item{
            id: createTransaction

            anchors {
            left: parent.left
            right: parent.right
            rightMargin: 5
            top: keyDisplay.bottom
            topMargin: 10
            }
            
            height: childrenRect.height

            RowLayout{
                anchors.fill: parent



                ColumnLayout{
                    Layout.fillHeight: true

                    RowLayout{
                        Text{
                            text: "Amount"
                            color: "white"
                            Layout.preferredWidth: 50
                            horizontalAlignment: Text.AlignHCenter
                        }

                        TextField{
                            id: amountField
                            Layout.preferredWidth: 100
                        }
                    }

                    RowLayout{
                        Text{
                            text: "Fee"
                            color: "white"
                            Layout.preferredWidth: 50
                            horizontalAlignment: Text.AlignHCenter
                        }

                        TextField{
                            id: feeField
                            Layout.preferredWidth: 100
                        }
                    }
                }

                Text{
                    text: "Receiver Address"
                    color: "white"
                }

                TextField{
                    id: receiver
                    Layout.fillWidth:true
                }

                Button{
                    id: transactionBtn
                    text: "Create\nTransaction"
                    onClicked: {
                        BlockchainApp.createTransaction(pubKey.text, privKey.text, receiver.text,
                            parseFloat(amountField.text), parseFloat(feeField.text))
                    }
                }

                Button {
                    text: "Validate\nSignature"
                    onClicked: BlockchainApp.validateTransaction()
                }

            }
        }

      
    }
}
