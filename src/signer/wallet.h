#include "signer.h"

class Wallet : Signer {

    Wallet()  {

        pub_key = get_pub_key();

    };

public:
    std::vector<unsigned char> pub_key;

}
