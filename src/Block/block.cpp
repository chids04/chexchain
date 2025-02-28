#include "block.h"
#include "hashtools.h"
#include "utility.h"

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
    timestamp = Utility::genTimeStamp();
    hash = createHash();
}

Block::Block()
{
    index = 0;
    prev_hash = "";
    timestamp = Utility::genTimeStamp();
    hash = createHash();
}

std::string Block::createHash()
{
    std::string input = std::to_string(index) + std::to_string(timestamp) + prev_hash;
    std::string str_hash = HashCode::genSHA256(input);

    return str_hash;
}

std::string Block::getInfo()
{
    //getting timestamp in readable format
    std::string time_str = Utility::printTime(timestamp);

    return "Block Index: " + std::to_string(index) + "\t" + "Timestamp: " + time_str + "\n" +
            "Hash: " + hash + "\n" + "Previous Hash: " + prev_hash;
}
