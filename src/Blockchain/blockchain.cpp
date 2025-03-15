#include "blockchain.h"
#include "block.h"
#include "wallet.h"
#include "transaction.h"

#include <algorithm>
#include <iterator>

using namespace BlockchainAssignment::Wallet;

Blockchain::Blockchain()
{
    blocks.push_back(std::make_shared<Block>());
}

void Blockchain::generateBlock()
{
    int x;
    std::vector<std::unique_ptr<Transaction>> to_process;
    
    if(transactionPool.size() < MAX_TRANSACTIONS){
        x = transactionPool.size();
    }
    else{
        x = MAX_TRANSACTIONS;
    }

    to_process.reserve(x);

    //move ownership of transaction to this temp array, which will then be moved to the block, moving better than copying
    std::move(transactionPool.begin(), transactionPool.begin() + x, std::back_inserter(to_process));
    transactionPool.erase(transactionPool.begin(), transactionPool.begin() + x);

    blocks.push_back(std::make_shared<Block>(blocks.back(), std::move(to_process)));
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


