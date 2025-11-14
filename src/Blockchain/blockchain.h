#pragma once

#include "block.h"
#include "transaction.h"

#include <vector>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

constexpr int MAX_TRANSACTIONS = 5;

struct TransactionComparator {
    bool operator()(const Transaction& a, const Transaction& b) const {
        return a.fee < b.fee;
    }
};

class Blockchain{

public:

    enum class BlockchainErrorType{ None, NoBlocks, HashMismatch, MerkleRootMismatch, SignatureMismatch };
    unsigned int BLOCK_DIFFICULTY_LEVEL = 4;

    struct BlockchainError{
        BlockchainErrorType type;
        int blockIndex;
        int txIndex;
    };

    Blockchain();
    ~Blockchain();

    void spawnMiners(int num);
    std::optional<std::vector<Transaction>> getWork(unsigned maxTx);
    void publishBlock(std::shared_ptr<Block> b);
    int getDifficulty();

    void generateBlock(const std::string &miner_address);
    std::optional<std::shared_ptr<Block>> getLastBlock();
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



    //testing functions

    bool invalidateHash();
    bool invalidateTxHash();
    bool invalidateTxSig();


private:

    mutable std::mutex txMutex_;
    mutable std::mutex blockMutex_;
    mutable std::mutex difficutlyMutex_;

    std::vector<std::shared_ptr<Block>> blocks;
    std::vector<Transaction> transactionPool;


};
