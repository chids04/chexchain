#include "blockchainapp.h"
#include "blockchain.h"
#include "wallet.h"
#include <stdexcept>
#include <string>


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

void BlockchainApp::checkBalance(const QString &address) {
    
    float bal = blockchain.checkBalance(address.toStdString());

    emit printMsg("Wallet balance is " + QString::fromStdString(std::to_string(bal)));
}

void BlockchainApp::createTransaction(const QString &sender, const QString &privKey, const QString &receiver,
    double amount, double fee)
{
    auto senderStr = sender.toStdString();
    auto receiverStr = receiver.toStdString();
    auto privKeyStr = privKey.toStdString();

    try{
        auto res = blockchain.createTransaction(senderStr, privKeyStr, receiverStr, amount, fee);
        emit printMsg(QString::fromStdString(blockchain.createTransaction(senderStr, privKeyStr, receiverStr, amount, fee)));
    }
    catch(const std::runtime_error &ex){
        emit printMsg(QString::fromStdString(ex.what()));
    }
    
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

void BlockchainApp::invalidateTxSigs() {
    bool success = blockchain.invalidateTxSig();

    if(success){
        emit printMsg("All transaction sigantures invalidated");
    }
    else{
        emit printMsg("Failed to invalidate transcation signatures");
    }
}

void BlockchainApp::invalidateBlockHash() {
    bool success = blockchain.invalidateHash();
    if(success){
        emit printMsg("All block hashes were invalidated");
    }
    else{
        emit printMsg("Failed to invalided all block hashes");
    }
}

void BlockchainApp::invalidateTxHashes() {
    bool success = blockchain.invalidateTxHash();

    if(success){
        emit printMsg("All transaction signatures were invalidated");
    }
    else{
        emit printMsg("Failed to invalidate all transaction signatures");
    }
}

void BlockchainApp::readAllBlocks()
{
    emit printMsg(QString::fromStdString(blockchain.readAllBlocks()));
}

void BlockchainApp::generateBlock(const QString &miner_address)
{
    auto minerAddressStr = miner_address.toStdString();

    blockchain.generateBlock(minerAddressStr);
}

void BlockchainApp::validateBlockchain() {
    auto error = blockchain.validateBlockchain();
    if(error.type == Blockchain::BlockchainErrorType::None){
        emit printMsg("All blocks and transactions are valid");
    }
    else{
        
        if(error.type == Blockchain::BlockchainErrorType::HashMismatch){
            emit printMsg("Hash mismatch at block index " + QString::number(error.blockIndex));
        }

        else if(error.type == Blockchain::BlockchainErrorType::MerkleRootMismatch){
            emit printMsg("Merkle root mismatch at block index " + QString::number(error.blockIndex));
        }

        else if(error.type == Blockchain::BlockchainErrorType::SignatureMismatch){
            emit printMsg(QString("Signature error in block at index %1 with transaction index %2").arg(error.blockIndex, error.txIndex));
        }
    }

    
}
