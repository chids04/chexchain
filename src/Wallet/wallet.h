#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/objects.h>
#include <openssl/pem.h>
#include <vector>

namespace BlockchainAssignment::Wallet {
    
    class Wallet {
    public:
        std::string publicID;
        std::string privateKey;

        Wallet(); // Constructor generates keys

        static bool ValidatePrivateKey(const std::string& privateKey, const std::string& publicID);
        static bool ValidateSignature(const std::string& publicID, const std::string& datahash, const std::string& datasig);
        static std::string CreateSignature(const std::string& publicID, const std::string& privateKey, const std::string& datahash);
    
    private:
        static EC_KEY* CreateKey(const std::string& publicID, const std::string& privateKey = "");
        static std::string Base64Encode(const std::vector<unsigned char>& data);
        static std::vector<unsigned char> Base64Decode(const std::string& encoded);
    };

} 

#endif // WALLET_H
