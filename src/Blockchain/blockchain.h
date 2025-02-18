#pragma once

#include "block.h"

#include <vector>
#include <memory>


class Blockchain{

public:
    Blockchain();
    std::string blockInfo(int index);

private:
    std::vector<std::shared_ptr<Block>> blocks;

};