#include "blockchain.h"
#include "block.h"
#include "wallet.h"
#include "transaction.h"

#include <algorithm>
#include <string>

using namespace BlockchainAssignment::Wallet;

Blockchain::Blockchain()
{
    blocks.push_back(std::make_shared<Block>());
}

void Blockchain::generateBlock(const std::string &miner_address)
{
    int x = std::min(static_cast<int>(transactionPool.size()), MAX_TRANSACTIONS);
    std::vector<std::unique_ptr<Transaction>> to_process;

    to_process.reserve(x);

    //move ownership of transaction to this temp array, which will then be moved to the block, moving better than copying
    //in c++ u can create virtual heap over the vector, so that i can efficiently sort transactions for miners to pick
    for(int i=0; i<x; ++i){
        std::pop_heap(transactionPool.begin(), transactionPool.end(), TransactionComparator());
        to_process.push_back(std::move(transactionPool.back()));
        transactionPool.pop_back();
    }

    blocks.push_back(std::make_shared<Block>(blocks.back(), miner_address, std::move(to_process)));
}

std::string Blockchain::readAllBlocks()
{
    std::string info;

    for(const auto &block : blocks){
        info += block->getInfo() + "\n\n";
    }

    return info;
}

std::string Blockchain::blockInfo(int index)
{
    if(index < 0 || index >= blocks.size()){
        return "invalid index";
    }
    else{
        return blocks[index]->getInfo();
    }
}

std::string Blockchain::createTransaction(const std::string &sender, const std::string &privKey, const std::string &receiver,
    double amount, double fee)
{
    auto transaction = std::make_unique<Transaction>(sender, receiver, privKey, amount, fee);
    std::string log = transaction->printTransaction();
    
    transactionPool.push_back(std::move(transaction));
    std::push_heap(transactionPool.begin(), transactionPool.end(), TransactionComparator());


    return log;

}

std::string Blockchain::printPendingTransactions()
{
    std::string msg = "Pending Transactions: " + std::to_string(transactionPool.size()) + "\n\n";

    for(const auto &tx : transactionPool){
        msg += tx->printTransaction() + "\n\n";
    }

    return msg;
}

std::pair<std::string,std::string>Blockchain::generateWallet()
{
    std::string priv_key;
    Wallet wallet(priv_key);

    return std::make_pair(priv_key, wallet.publicID);
}

std::string Blockchain::validateWallet(const std::string & priv_key, const std::string & pub_key)
{
    if(Wallet::ValidatePrivateKey(priv_key, pub_key)){
        return "This is a valid private/public key pair";
    }
    else{
        return "This is an invalid private/public key pair";
    }
}


