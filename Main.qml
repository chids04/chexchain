import QtQuick 
import QtQuick.Controls 

import blockchain

ApplicationWindow {
    visible: true
    width: 680
    height: 500
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

        Item{
            id: keyDisplay

            height:childrenRect.height
            anchors{
                left: blockIndex.right
                right: parent.right
                leftMargin: 10
                top: blockIndex.top
            }

            Label{
                id: pubKeyText
                text: "Public Key:"
                color: "white"

                TextMetrics{
                    id: pubMetrics
                    text: pubKeyText.text
                }

                anchors {
                    left: parent.left
                    top: parent.top
                }

                TextField{
                    id: pubKey
                    width: keyDisplay.width - pubMetrics.width - genWallet.width - 18
                    anchors {
                        left:  pubKeyText.right
                        leftMargin: 8
                        right: keyDisplay.right
                        top: pubKeyText.top
                        bottom: pubKeyText.bottom
                    }

                    Connections{
                        target: BlockchainApp
                        function onShowPubKey(key){
                            pubKey.text = key
                        }
                    }
                }

                Button{
                    id: genWallet
                    text: "Generate\nWallet"
                    height: pubKey.height + privKey.height + 10
                    onClicked: BlockchainApp.generateWallet()
                    anchors{
                        left: pubKey.right
                        leftMargin: 5
                    }
                }

                Button{
                    id: validateKeys
                    text: "Validate\nKey Pair"

                    anchors{
                        top: genWallet.bottom
                        topMargin: 5
                        horizontalCenter: genWallet.horizontalCenter
                    }

                    onClicked: BlockchainApp.validateWallet(privKey.text,pubKey.text)
                }
            }

            Label{
                id: privKeyText
                text: "Private Key:"
                color: "white"

                TextMetrics{
                    id: privMetrics
                    text: privKeyText.text
                }

                anchors {
                    top: pubKeyText.bottom
                    topMargin: 5
                    left: parent.left
                }

                TextField{
                    id: privKey
                    
                    width: keyDisplay.width - privMetrics.width  - genWallet.width - 15
                    anchors {

                        left: privKeyText.right
                        right: keyDisplay.right
                        top: privKeyText.top
                        bottom: privKeyText.bottom
                        leftMargin: 5
                    }

                    Connections{
                        target: BlockchainApp
                        function onShowPrivKey(key){
                            privKey.text = key
                        }
                    }
                }
            }
        }

        Item{
            id: createTransaction

            anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            bottomMargin: 5
            }
            
            height: childrenRect.height

            Button{
                id: transactionBtn
                text: "Create\nTransaction"
                anchors{
                    left: parent.left
                    top: parent.top
                }

                onClicked: {

                    BlockchainApp.createTransaction(pubKey.text, privKey.text, receiver.text, 
                        parseFloat(amountField.text), parseFloat(feeField.text))
                }
            }

            Item{
                id: amounts

                height: childrenRect.height
                width: childrenRect.width

                anchors{
                    leftMargin: 5
                    left: transactionBtn.right
                    top: parent.top
                }

                Label{

                    id: amountText
                    text: "Amount:"
                    color: "white"

                    anchors{
                        left: parent.left
                        top: parent.top
                    }

                    TextField{
                        id: amountField
                        width: 40

                        anchors{
                            left: parent.right
                            leftMargin: 3
                        }
                    }

                    Label{
                        id: feeText
                        text: "Fee:"
                        color: "white"
                        anchors{
                            topMargin: 3
                            top: amountText.bottom
                            left: parent.left
                        }

                    }

                    TextField{
                        id: feeField
                        width: 40

                        anchors {
                            left: amountField.left
                            topMargin: 3
                            top: amountField.bottom

                        }

                        Label{
                            text: "Reciever Address:"
                            color: "white"

                            anchors{
                                left: parent.right
                                leftMargin: 5
                                top: parent.top
                            }

                            TextField{
                                id: receiver
                                width: createTransaction.width - 300
                                anchors{
                                    left: parent.right
                                    leftMargin: 3
                                }
                            }
                        }
                    }

                }

                
            }

            

        }

        // Button{
        //     id: genWallet
        //     text: "Generate\nWallet"
        //     width: 30
        //     height: 30
        //     anchors{
        //         left: keyDisplay.right
        //         right: parent.right
        //         top: parent.bottom
        //     }
        // }
        


    }
}
