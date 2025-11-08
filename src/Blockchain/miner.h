#pragma once

#include "blockchain.h"
#include "wallet.h"

#include <iostream>
#include <thread>
#include <stop_token>
#include <print>

using namespace BlockchainAssignment::Wallet;

struct KeyPair{
    std::string pub_key;
    std::string priv_key;
};

class Miner {

public:
    Miner(Blockchain &chain) : chain_(chain){
        Wallet wallet(keys.priv_key);
        keys.pub_key = wallet.publicID;

        };

    // void operator()(std::stop_token st)
    // {
    //     while (!st.stop_requested()) {

    //         auto work = chain_.getWork(MAX_TRANSACTIONS);
    //         auto prev_block_opt = chain_.getLastBlock();

    //         std::shared_ptr<Block> prev_block = prev_block_opt.value_or(nullptr);



    //         auto start = std::chrono::high_resolution_clock::now();

    //         auto blk   = std::make_shared<Block>(prev_block,
    //                                             keys.pub_key,
    //                                             std::move(work),
    //                                             chain_.BLOCK_DIFFICULTY_LEVEL);

    //         std::println("found a block with hash {}", blk->hash);

    //         auto stop  = std::chrono::high_resolution_clock::now();

    //         chain_.publishBlock(std::move(blk));
    //     }
    // }

private:
    KeyPair keys;
    unsigned    id_;
    Blockchain& chain_;
    std::string payout_;
};
