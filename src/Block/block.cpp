#include "block.h"
#include "hashtools.h"
#include "transaction.h"
#include "utility.h"

#include <memory>
#include <string>
#include <vector>
#include <openssl/sha.h>

using namespace BlockchainAssignment;

Block::Block(std::shared_ptr<Block> prev_block, std::string miner_address, std::vector<std::unique_ptr<Transaction>> tx)
{
    prev_hash = prev_block->hash;
    index = prev_block->index + 1;
    timestamp = Utility::genTimeStamp();
    hash = createHash();
    auto mine_tx = std::make_unique<Transaction>("Mine Rewards", miner_address, "", block_reward + MINE_REWARD, 0);
    transactions.push_back(std::move(mine_tx));

     transactions.insert(
        transactions.end(),
        std::make_move_iterator(tx.begin()),
        std::make_move_iterator(tx.end())
    );

    for(const auto &tx: transactions){
        block_reward += tx->fee;
    }


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
    std::string input = std::to_string(index) + std::to_string(timestamp) + std::to_string(nonce) + 
        prev_hash + std::to_string(block_reward);

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
    int size = transactions.size();
    std::string msg;

    if(index == 0){
        msg =  "Block Index: " + std::to_string(index) + "\t\t" + "Timestamp: " + time_str + "\n";
    }

    else{

        
        msg =  "Block Index: " + std::to_string(index) + "\t\t" + "Timestamp: " + time_str + "\n" +
            "Hash: " + hash + "\n" + "Previous Hash: " + prev_hash + "\nNonce: " + std::to_string(nonce) + 
            + "\nDifficulty: " + std::to_string(DIFFICULTY_THRESHOLD) + "\nMiner Address: " + transactions[0]->receiver + "\nTotal Block Reward: " + std::to_string(block_reward) +
            "\nFees Recieved: " + std::to_string(std::max(block_reward-MINE_REWARD, MINE_REWARD))+ "\nTransactions: " + std::to_string(transactions.size()) + "\n\n" ;

    }
    for(const auto &tx : transactions){
        msg += tx->printTransaction() + "\n\n";
    }

    return msg;
}

std::vector<std::unique_ptr<Transaction>> Block::getTransactions() {
    return transactions;
}

