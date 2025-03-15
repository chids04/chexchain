#pragma once
#include <string>
#include <memory>
#include <vector>

#include "transaction.h"

constexpr int DIFFICULTY_THRESHOLD = 4;

class Block{

public:
    Block(std::shared_ptr<Block> prev_block, std::vector<std::unique_ptr<Transaction>> tx);
    Block(); //genesis block

    long long timestamp;
    int index;
    int nonce = 0;
    std::string hash;

    std::string prev_hash;

    std::string createHash();
    std::string getInfo();

private:
    std::vector<std::unique_ptr<Transaction>> transactions;
    void mine();

    
};