#pragma once

#include "block.h"
#include "transaction.h"

#include <vector>
#include <memory>


constexpr int MAX_TRANSACTIONS = 5;

struct TransactionComparator {
    bool operator()(const std::unique_ptr<Transaction>& a, const std::unique_ptr<Transaction>& b) const {
        return a->fee < b->fee;
    }
};

class Blockchain{

public:

    enum class BlockchainErrorType{ None, HashMismatch, MerkleRootMismatch, SignatureMismatch };

    struct BlockchainError{
        BlockchainErrorType type;
        int blockIndex;
        int txIndex;
    };

    Blockchain();
    void generateBlock(const std::string &miner_address);
    std::string readAllBlocks();
    std::string blockInfo(int index);
    BlockchainError validateBlockchain();

    float checkBalance(const std::string& address);
    std::string createTransaction(const std::string &sender, const std::string &privKey, const std::string &receiver,
        double amount, double fee);
    std::string printPendingTransactions();
    std::string validateTransaction();

    std::pair<std::string, std::string> generateWallet();
    std::string validateWallet(const std::string &priv_key, const std::string &pub_key);


private:
    std::vector<std::shared_ptr<Block>> blocks;
    std::vector<std::unique_ptr<Transaction>> transactionPool;

};

