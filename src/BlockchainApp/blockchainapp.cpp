#include "blockchainapp.h"


void BlockchainApp::init()
{
    emit printMsg("Blockchain Initalised");
}

void BlockchainApp::getBlockInfo(int index){
    std::string blockInfo = blockchain.blockInfo(index);
    
    emit printMsg(QString::fromStdString(blockInfo));
}

