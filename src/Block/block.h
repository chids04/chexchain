#pragma once
#include <string>

class Block{

public:
    Block(Block *block);
    Block(); //genesis block

    long long timestamp;
    int index;
    std::string hash;
    std::string prev_hash;

    std::string createHash();
    std::string getInfo();
    
};