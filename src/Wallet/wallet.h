#pragma once

#include <string>
#include <vector>
#include <openssl/ec.h>

namespace BlockchainAssignment {
    namespace Wallet {
        class Wallet {
        public:
            // Public ID - viewable to all (Derived from the user's private key)
            std::string publicID;
            
            // Constructor that generates a new wallet
            // privateKey will be populated with the generated private key
            Wallet(std::string& privateKey);
            
            // Validate if a private key matches a public ID
            static bool ValidatePrivateKey(const std::string& privateKey, const std::string& publicID);
            
            // Validates if a signature is legitimate
            static bool ValidateSignature(std::string publicID, const std::string& datahash, const std::string& datasig);
            
            // Creates a signature using the private key
            static std::string CreateSignature(const std::string& publicID, const std::string& privateKey, const std::string& datahash);
            
        private:
            // Creates an EC_KEY from a public ID and optional private key
            static EVP_PKEY* createKey(const std::string& publicID, const std::string& privateKey = "");
            
            // Base64 encoding and decoding utility functions
            static std::string base64_encode(const std::vector<unsigned char>& input);
            static std::vector<unsigned char> base64_decode(const std::string& input);
        };
    }
}