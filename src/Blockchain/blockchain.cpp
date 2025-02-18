#include "blockchain.h"
#include "block.h"

Blockchain::Blockchain()
{
    blocks.push_back(std::make_shared<Block>());
}

std::string Blockchain::blockInfo(int index)
{
    if(index < 0 || index >= blocks.size()){
        return "invalid index";
    }
    else{
        return blocks[index]->getInfo();
    }
}
