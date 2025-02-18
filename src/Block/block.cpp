#include "block.h"
#include "hashtools.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

using namespace BlockchainAssignment;

Block::Block(Block *block)
{
    prev_hash = block->prev_hash;
    index = block->index + 1;

    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    timestamp = duration.count();

    hash = createHash();
}

Block::Block()
{
    index = 0;
    prev_hash = "";

    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    timestamp = duration.count();

    hash = createHash();
}

std::string Block::createHash()
{
    std::string input = std::to_string(index) + std::to_string(timestamp) + prev_hash;
    
    std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);

    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, input.data(), input.size());
    SHA256_Final(hash.data(), &ctx);

    std::string str_hash = HashCode::ByteArrayToString(hash);

    return str_hash;
}

std::string Block::getInfo()
{
    //getting timestamp in readable format

    std::chrono::seconds  seconds(timestamp/1000); //convert ms to s
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(0) + seconds);
    
    std::tm* localTime = std::localtime(&time);
    std::ostringstream oss;

    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");

    return "Block Index: " + std::to_string(index) + "\t" + "Timestamp: " + oss.str() + "\n" +
            "Hash: " + hash + "\n" + "Previous Hash: " + prev_hash;
}
