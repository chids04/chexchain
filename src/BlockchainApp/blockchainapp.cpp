#include "blockchainapp.h"
#include "wallet.h"


using namespace BlockchainAssignment::Wallet;

void BlockchainApp::init()
{
    emit printMsg("Blockchain Initalised");
}

void BlockchainApp::getBlockInfo(int index){
    std::string blockInfo = blockchain.blockInfo(index);
    
    emit printMsg(QString::fromStdString(blockInfo));
}

void BlockchainApp::generateWallet()
{
    auto [priv_key, pub_key] = blockchain.generateWallet();

    emit showPrivKey(QString::fromStdString(priv_key));
    emit showPubKey(QString::fromStdString(pub_key));
}

void BlockchainApp::validateWallet(const QString &priv, const QString &pub)
{
    std::string pub_key = pub.toStdString();
    std::string priv_key = priv.toStdString();

    emit printMsg(QString::fromStdString(blockchain.validateWallet(priv_key, pub_key)));
}

void BlockchainApp::createTransaction(const QString &sender, const QString &privKey, const QString &receiver,
    double amount, double fee)
{
    std::string senderStr = sender.toStdString();
    std::string receiverStr = receiver.toStdString();
    std::string privKeyStr = privKey.toStdString();
    
    emit printMsg(QString::fromStdString(blockchain.createTransaction(senderStr, privKeyStr, receiverStr, amount, fee)));
}

void BlockchainApp::validateTransaction()
{
    // bool isValid = Wallet::Wallet::ValidateSignature(transaction.sender, transaction.hash, transaction.sig);

    // if(isValid){
    //     emit printMsg("Transaction has a valid signature");
    // }
    // else{
    //     emit printMsg("Transaction has an invalid signature");
    // }
}

void BlockchainApp::printPendingTransactions()
{
    emit printMsg(QString::fromStdString(blockchain.printPendingTransactions()));
}

void BlockchainApp::readAllBlocks()
{
    emit printMsg(QString::fromStdString(blockchain.readAllBlocks()));
}

void BlockchainApp::generateBlock()
{
    blockchain.generateBlock();
}
