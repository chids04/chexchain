#ifndef HASHTOOLS_H
#define HASHTOOLS_H

#include <string>
#include <vector>
#include <cstdint>

namespace BlockchainAssignment::HashCode {

    // converts a byte vector to a hexadecimal string
    std::string ByteArrayToString(const std::vector<uint8_t>& bytes);

    // converts a hexadecimal string to a byte vector
    std::vector<uint8_t> StringToByteArray(const std::string& hex);

    // combines two SHA-256 hashes into a new SHA-256 hash
    std::string CombineHash(const std::string& hash1, const std::string& hash2);

    //generates SHA-256 and returns it as a hex string
    std::string genSHA256(const std::string &input);

}  // namespace BlockchainAssignment::HashCode

#endif // HASHTOOLS_H
