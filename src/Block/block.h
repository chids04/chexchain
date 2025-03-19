#pragma once
#include <string>
#include <memory>
#include <vector>

#include "transaction.h"

constexpr int DIFFICULTY_THRESHOLD = 4;
constexpr float MINE_REWARD = 50;

class Block{

public:
    Block(std::shared_ptr<Block> prev_block, std::string miner_address, std::vector<std::unique_ptr<Transaction>> tx);
    Block(); //genesis block

    long long timestamp;
    int index;
    int nonce = 0;
    float block_reward = 0;
    std::string hash;

    std::string prev_hash;

    std::string createHash();
    std::string getInfo();

    std::vector<std::unique_ptr<Transaction>> getTransactions();

private:
    std::vector<std::unique_ptr<Transaction>> transactions;
    void mine();

    
};