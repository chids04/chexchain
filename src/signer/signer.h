#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/core_names.h>

#include <array>
#include <vector>

const static std::string HASH_NAME = "SHA3-256";

class Signer {

public:
    Signer();
    std::vector<unsigned char> sign(std::vector<unsigned char> data);
    std::array<unsigned char, 20> pub_key;

    static bool validate_sig(Signer* signer, );

private:
    EVP_PKEY *priv_key;

};
