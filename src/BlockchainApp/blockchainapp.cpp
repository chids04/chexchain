#include "blockchainapp.h"
#include "wallet.h"
#include "transaction.h"


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
    
    std::string priv_key;
    Wallet wallet(priv_key);

    emit showPrivKey(QString::fromStdString(priv_key));
    emit showPubKey(QString::fromStdString(wallet.publicID));
}

void BlockchainApp::validateWallet(const QString &priv, const QString &pub)
{
    std::string pub_key = pub.toStdString();
    std::string priv_key = priv.toStdString();

    if(Wallet::Wallet::ValidatePrivateKey(priv_key, pub_key)){
        emit printMsg("This is a valid private/public key pair");
    }
    else{
        emit printMsg("This is an invalid private/public key pair");
    }
}

void BlockchainApp::createTransaction(const QString &sender, const QString &privKey, const QString &receiver, 
    double amount, double fee)
{
    std::string senderStr = sender.toStdString();
    std::string receiverStr = receiver.toStdString();
    std::string privKeyStr = privKey.toStdString();
    
    Transaction transaction(senderStr, receiverStr, privKeyStr, amount, fee);

    std::string log = transaction.printTransaction();
    emit printMsg(QString::fromStdString(log));
    
}
