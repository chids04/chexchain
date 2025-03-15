#include "block.h"
#include "hashtools.h"
#include "utility.h"

#include <string>
#include <vector>
#include <openssl/sha.h>

using namespace BlockchainAssignment;

Block::Block(std::shared_ptr<Block> prev_block, std::vector<std::unique_ptr<Transaction>> tx)
{
    prev_hash = prev_block->hash;
    index = prev_block->index + 1;
    timestamp = Utility::genTimeStamp();
    hash = createHash();
    transactions = std::move(tx);
    mine();
}


Block::Block()
{
    index = 0;
    prev_hash = "";
    timestamp = Utility::genTimeStamp();
}

std::string Block::createHash()
{
    std::string input = std::to_string(index) + std::to_string(timestamp) + std::to_string(nonce) + prev_hash;
    std::string str_hash = HashCode::genSHA256(input);

    return str_hash;
}

void Block::mine()
{
    std::string zeros(DIFFICULTY_THRESHOLD, '0');

    hash = createHash();        
    std::string hash_zeros = hash.substr(0, DIFFICULTY_THRESHOLD);

    if(hash_zeros == zeros){
        return;
    }

    bool isValid = false;

    while(!isValid){
        nonce++;
        hash = createHash();
        hash_zeros = hash.substr(0, DIFFICULTY_THRESHOLD);

        if(hash_zeros == zeros){
            isValid = true;
        }
    }

    
}

std::string Block::getInfo()
{
    //getting timestamp in readable format
    std::string time_str = Utility::printTime(timestamp);

    std::string msg =  "Block Index: " + std::to_string(index) + "\t\t" + "Timestamp: " + time_str + "\n" +
            "Hash: " + hash + "\n" + "Previous Hash: " + prev_hash + "\nNonce: " + std::to_string(nonce) + 
            + "\nDifficulty: " + std::to_string(DIFFICULTY_THRESHOLD) + "\nTransactions: " + std::to_string(transactions.size()) + "\n\n";

    for(const auto &tx : transactions){
        msg += tx->printTransaction() + "\n\n";
    }

    return msg;
}

