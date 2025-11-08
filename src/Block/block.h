#pragma once
#include <string>
#include <memory>
#include <vector>

#include "transaction.h"

constexpr float MINE_REWARD = 50;

class Block{

public:
    Block(std::shared_ptr<Block> prev_block, std::string miner_address,
        std::vector<Transaction> tx, unsigned int difficulty);

    Block(); //genesis block

    long long timestamp;
    int index;
    int nonce = 0;
    unsigned int difficulty;
    float block_reward = 0;
    std::string hash;

    std::string prev_hash;

    std::string createHash();
    std::string getInfo() const;
    std::string merkle_root;

    std::vector<Transaction> transactions;


    /* gets the required sibling hashes needed to reconstruct the merkle root
    in bitcoin, this is used by verification nodes to verify a transaction in a block
    they request the merkle proof from a full node and verify the transaction is in the merkle root using the merkle proof
     */
    std::vector<std::pair<std::string, bool>> getMerkleProof(int index);
    static std::string computeMerkleRoot(const std::vector<Transaction> &transactions);

    //testing functions
    void invalidateTxHash();
    void invalidateTxSig();




private:
    void mine();
    void mineParallel(int numThreads);


};
