#include "blockchain.h"
#include "block.h"
#include "wallet.h"
#include "transaction.h"
#include "miner.h"

#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <print>
#include <mutex>

using namespace BlockchainAssignment::Wallet;

//generates the genesis block
Blockchain::Blockchain()
{
    blocks.push_back(std::make_shared<Block>());
    spawnMiners(3);

    //set up some miners here

    
}

Blockchain::~Blockchain()
{
    minerStopSource.request_stop();
}

void Blockchain::spawnMiners(int num) { 
    //stop source allows us to send stop signal to all running threads (miners)

    if(num > std::thread::hardware_concurrency()){
        std::println("Too many miners for num of threads");
        return;
    }

    //reset threads and stop source
    minerStopSource.request_stop();
    minerThreads.clear();
    minerStopSource = {};

    minerThreads.reserve(num);
    
    for(int i=0; i<num; ++i){
        minerThreads.emplace_back(
            Miner{*this},
            minerStopSource.get_token()
        );
    }


}

std::vector<std::unique_ptr<Transaction>> Blockchain::getWork(unsigned maxTx)
{
    std::scoped_lock lk(txMutex_);

    unsigned n = std::min<unsigned>(transactionPool.size(), maxTx);
    std::vector<std::unique_ptr<Transaction>> out;
    out.reserve(n);

    for (unsigned i = 0; i < n; ++i) {
        std::pop_heap(transactionPool.begin(), transactionPool.end(),
                      TransactionComparator());            // highest-fee tx to back
        out.push_back(std::move(transactionPool.back()));  // move, not copy
        transactionPool.pop_back();
    }

    return out;
}

void Blockchain::publishBlock(std::shared_ptr<Block> b){
    std::scoped_lock lk(blockMutex_, txMutex_);

    auto prev = blocks.back();

    if(b->prev_hash != prev->hash){
        //move transactions back into the mempool;
        auto txs = b->removeTransactions();

        //remove block reward transaction
        if (!txs.empty()) {
            txs.erase(txs.begin()); 
        }

        transactionPool.insert(transactionPool.end(), std::make_move_iterator(txs.begin()), std::make_move_iterator(txs.end()));
        std::push_heap(transactionPool.begin(), transactionPool.end(), TransactionComparator());
        
    }
    else{
        std::println("New block with prev hash {} and hash {}", b->prev_hash, b->hash);
        blocks.push_back(std::move(b));
    }
    
}

int Blockchain::getDifficulty() {
    std::scoped_lock lk(difficutlyMutex_);
    return BLOCK_DIFFICULTY_LEVEL;
}

//generates a blocks
void Blockchain::generateBlock(const std::string &miner_address)
{
    int x = std::min(static_cast<int>(transactionPool.size()), MAX_TRANSACTIONS);
    std::vector<std::unique_ptr<Transaction>> to_process;

    to_process.reserve(x);

    //move ownership of transaction to this temp array, which will then be moved to the block, moving better than copying
    //in c++ u can create virtual heap over the vector, so that i can efficiently sort transactions for miners to pick
    for(int i=0; i<x; ++i){
        std::pop_heap(transactionPool.begin(), transactionPool.end(), TransactionComparator());
        to_process.push_back(std::move(transactionPool.back()));
        transactionPool.pop_back();
    }
    
    //could expand this to take a weighted average of x amount of transactions
    auto start = std::chrono::high_resolution_clock::now();
    auto block = std::make_shared<Block>(blocks.back(), miner_address, std::move(to_process), BLOCK_DIFFICULTY_LEVEL);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    if (duration.count() > 30) {
        if(BLOCK_DIFFICULTY_LEVEL > 0){
            BLOCK_DIFFICULTY_LEVEL--;
        }
    }
    else if(duration.count() < 40){
        BLOCK_DIFFICULTY_LEVEL++;
    }

    blocks.push_back(block);
}

std::optional<std::shared_ptr<Block>> Blockchain::getLastBlock() {
    if(blocks.size() > 0){
        return blocks.back();
    }
    else{
        return std::nullopt;
    }
}

//reads all the blocks
std::string Blockchain::readAllBlocks()
{
    std::scoped_lock lk(blockMutex_);
    std::string info;

    for(const auto &block : blocks){
        info += block->getInfo() + "\n\n";
   }

    return info;
}


//returns info of block at specific index
std::string Blockchain::blockInfo(int index)
{
    std::scoped_lock lk(blockMutex_);
    if(index < 0 || index >= blocks.size()){
        return "invalid index";
    }
    else{
        return blocks[index]->getInfo();
    }
}

//validates blocks and if invalid, returns the first invalid block, else returns 0
auto Blockchain::validateBlockchain() -> BlockchainError
{
    std::scoped_lock lk(blockMutex_, txMutex_);
    for(int i=0; i<blocks.size(); i++){
        if(i == 0){
            continue;
        }
        else{
            //make sure the hashes are correct
            if(blocks[i-1]->hash != blocks[i]->prev_hash){
                return { BlockchainErrorType::HashMismatch, i};
            }

            //make sure the transactions make the same merkle root
            const auto &transactions = blocks[i]->getTransactions();
            if(Block::computeMerkleRoot(transactions) != blocks[i]->merkle_root){
                return { BlockchainErrorType::MerkleRootMismatch, i};
            }
            
            //verify the signature for each of the transactions
            int txIdx = 0;
            for(const auto& tx: transactions){
                if(Wallet::ValidateSignature(tx->sender, tx->hash, tx->sig) != true){
                    return { BlockchainErrorType::SignatureMismatch, i, txIdx};
                }
                txIdx++;
            }
        }
    }

    return {BlockchainErrorType::None, -1};
}

float Blockchain::checkBalance(const std::string& address) {
    //need to check transaction pool, and mined blocks, for current balance

    std::scoped_lock lk(blockMutex_);
    float bal = 0;

    for(const auto &pending_tx : transactionPool){
        if(pending_tx->receiver == address){
            bal += pending_tx->amount;                     
        }
        else if(pending_tx->sender == address){
            bal -= pending_tx->amount;
        }
    }

    for(const auto &b: blocks){
        const auto& transactions = b->getTransactions();

        for(const auto &tx : transactions){
            if(tx->receiver == address){
                bal += tx->amount;
            }
            else if(tx->sender == address){
                bal -= tx->amount;
            }
        }
    }

    return bal;
}

std::string Blockchain::createTransaction(const std::string &sender, const std::string &privKey, const std::string &receiver,
    double amount, double fee)
{
    std::scoped_lock lk(txMutex_);
    float bal = checkBalance(sender);

    if(bal < amount + fee){
        return "Sender has insufficient funds";
    }

    auto transaction = std::make_unique<Transaction>(sender, receiver, privKey, amount, fee);
    std::string log = transaction->printTransaction();
    
    //unique pointers used for transactions, since they should 'belong' to the block that mines it
    transactionPool.push_back(std::move(transaction));
    std::push_heap(transactionPool.begin(), transactionPool.end(), TransactionComparator());


    return log;

}

std::string Blockchain::printPendingTransactions()
{
    std::scoped_lock lk(txMutex_);
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

bool Blockchain::invalidateHash() {
    std::scoped_lock lk(blockMutex_);
    if(blocks.size() == 0){
        return false;
    }
    

    for(int i=0; i<blocks.size(); ++i){
        blocks[i]->hash = "badhash";
    }

    return true;

}

bool Blockchain::invalidateTxHash() {
    std::scoped_lock lk(txMutex_);
    //invalidate the transctions of all the blocks and check all blocks to make sure
    if(blocks.size() == 0){
        return false;
    }


    for(auto &b: blocks){
        b->invalidateTxHash();
        
    }

    return true;

}

bool Blockchain::invalidateTxSig()  {
    std::scoped_lock lk(txMutex_);
    if(blocks.size() == 0){
        return false;
    }



    for(auto &b: blocks){
        b->invalidateTxSig();
    }

    return true;
    

}


