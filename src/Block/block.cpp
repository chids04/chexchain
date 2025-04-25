#include "block.h"
#include "hashtools.h"
#include "transaction.h"
#include "utility.h"

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <print>
#include <thread>

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

    //can calculate block reward and being merkle root implementation
    for(const auto &tx: transactions){
        block_reward += tx->fee;
    }

    merkle_root = computeMerkleRoot(transactions);

    //mine();
    mineParallel(std::thread::hardware_concurrency());
}


Block::Block()
{
    index = 0;
    prev_hash = "";
    timestamp = Utility::genTimeStamp();
}

std::string Block::createHash()
{
    std::string input = std::to_string(index) + std::to_string(timestamp) + merkle_root + std::to_string(nonce) + 
        prev_hash + std::to_string(block_reward);

    std::string str_hash = HashCode::genSHA256(input);

    return str_hash;
}

void Block::mine()
{
    auto start = std::chrono::high_resolution_clock::now();
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

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::println("Mining took {} seconds (single-threaded)", elapsed.count());
}

void Block::mineParallel(int numThreads) {
    auto start = std::chrono::high_resolution_clock::now();
    std::string zeros(DIFFICULTY_THRESHOLD, '0');
    std::atomic<bool> found(false);
    std::atomic<int> foundNonce(0);

    // precompute static hash input
    std::string baseInput = std::to_string(index) + std::to_string(timestamp) + merkle_root + prev_hash + std::to_string(block_reward);

    auto worker = [&](int threadId, int batchSize) {
        int nonceStart = threadId * batchSize;
        int nonceEnd = nonceStart + batchSize;
        for (int localNonce = nonceStart; localNonce < nonceEnd && !found; ++localNonce) {
            std::string hashAttempt = HashCode::genSHA256(baseInput + std::to_string(localNonce));
            if (hashAttempt.substr(0, DIFFICULTY_THRESHOLD) == zeros) {
                if (!found.exchange(true)) {
                    hash = hashAttempt;
                    foundNonce = localNonce;
                }
                break;
            }
        }
    };

    std::vector<std::thread> threads;
    int batchSize = 10000; 
    std::println("Using {} threads", numThreads);
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, i, batchSize);
    }
    for (auto& t : threads) {
        t.join();
    }

    nonce = foundNonce;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::println("Mining took {} seconds (multi-threaded)", elapsed.count());
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
            "Hash: " + hash + "\n" + "Previous Hash: " + prev_hash +  "\nMerkle Root of Transactions: " + merkle_root + "\nNonce: " + std::to_string(nonce) + 
            + "\nDifficulty: " + std::to_string(DIFFICULTY_THRESHOLD) + "\nMiner Address: " + transactions[0]->receiver + "\nTotal Block Reward: " + std::to_string(block_reward) +
            "\nFees Recieved: " + std::to_string(std::max(block_reward-MINE_REWARD, MINE_REWARD))+ "\nTransactions: " + std::to_string(transactions.size()) + "\n\n" ;
    }

    for(const auto &tx : transactions){
        msg += tx->printTransaction() + "\n\n";
    }

    return msg;
}

const std::vector<std::unique_ptr<Transaction>>& Block::getTransactions() {
    return transactions;
}

std::vector<std::pair<std::string, bool>> Block::getMerkleProof(int txIndex) {
    std::vector<std::pair<std::string, bool>> proof;
    std::vector<std::string> leaf_nodes;

    //add leaf nodes (transaction hashes) to bottom of tree;
    for(const auto &tx : transactions){
        leaf_nodes.push_back(tx->hash);
    }

    //now we combine and hash, duplicating last item if order number of nodes
    while(leaf_nodes.size() > 1){
        std::vector<std::string> parent_nodes;
        for(int i=0; i<leaf_nodes.size(); i+=2){
            std::string left = leaf_nodes[i];
            std::string right;
            
            if(i+1 < leaf_nodes.size()){
                right = leaf_nodes[i+1];
            }
            else{
                right = left;
            }

            //now we check if left or right is a sibling to the transaction we are verifying
            if(i == txIndex || i+1 == txIndex){
                if(txIndex % 2 == 0){
                    /*
                    target index is left (even indexes always left node in binary tree)
                    so sibling is right
                    bool set to false to indicate on right
                    */
                    proof.push_back({right, false});
                }
                else{
                    //sibling is left, set bool to true to indicate this
                    proof.push_back({left, true});
                }
                
                txIndex = i / 2;
            }

            //we concat left right and hash to continue building tree, for next level
            std::string hash = HashCode::CombineHash(left, right);
            
            parent_nodes.push_back(hash);
        }

        //move onto next level
        leaf_nodes=parent_nodes;
    }

    //proof contains ordered list of siblings for verification
    return proof;
}

std::string Block::computeMerkleRoot(const std::vector<std::unique_ptr<Transaction>> &transactions) {
    std::vector<std::string> leaf_nodes;

    if(transactions.size() == 0){
        return "";
    }

    for(const auto &tx: transactions){
        //first add all hashes to an array
        leaf_nodes.push_back(tx->hash);
    }

    /*
    need to concat left and right node and then hash the result
    if odd number of transactions, then the last hash is hashed with itself
    final remaining hash is the merkle root
    */

    while(leaf_nodes.size() > 1){
        std::vector<std::string> parent_nodes;
        
        for(int i=0; i<leaf_nodes.size(); i+=2){
            std::string left = leaf_nodes[i];
            std::string right;

            if(i + 1 < leaf_nodes.size()){
                right = leaf_nodes[i + 1];
            }
            else{
                right = leaf_nodes[i];
            }

            //combine, hash then push to parent nodes list
            std::string combined_hash = HashCode::CombineHash(left, right);
            parent_nodes.push_back(combined_hash);
        }

        //update leaf_nodes to next level (parent_nodes)
        leaf_nodes = parent_nodes;
    }

    return leaf_nodes[0];
}

void Block::invalidateTxHash() {
    for(auto &tx : transactions){
        tx->hash = "badhash";
    }
}

void Block::invalidateTxSig() {
    for(auto &tx: transactions){
        tx->sig = "badsig";
    }
}




