#include "hashtools.h"

#include <sstream>
#include <iomanip>
#include <openssl/sha.h>  // OpenSSL for SHA-256
#include <algorithm>


namespace BlockchainAssignment::HashCode {

    std::string ByteArrayToString(const std::vector<uint8_t>& bytes) {
        std::ostringstream hex;
        for (uint8_t b : bytes) {
            hex << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
        }
        return hex.str();
    }

    std::vector<uint8_t> StringToByteArray(const std::string& hex) {
        std::vector<uint8_t> bytes;
        for (size_t i = 0; i < hex.length(); i += 2) {
            uint8_t byte = static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16));
            bytes.push_back(byte);
        }
        return bytes;
    }

    std::string CombineHash(const std::string& hash1, const std::string& hash2) {
        std::vector<uint8_t> bytes1 = StringToByteArray(hash1);
        std::vector<uint8_t> bytes2 = StringToByteArray(hash2);

        // Concatenate both byte vectors
        bytes1.insert(bytes1.end(), bytes2.begin(), bytes2.end());

        // Compute SHA-256 hash
        std::vector<uint8_t> combinedHash(SHA256_DIGEST_LENGTH);
        SHA256(bytes1.data(), bytes1.size(), combinedHash.data());

        return ByteArrayToString(combinedHash);
    }

}  