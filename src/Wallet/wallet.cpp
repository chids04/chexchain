#include "wallet.h"

#include <iostream>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>


namespace BlockchainAssignment::Wallet {

    Wallet::Wallet() {
        EC_KEY* key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
        if (!key || !EC_KEY_generate_key(key)) {
            std::cerr << "Key generation failed\n";
            return;
        }

        // Get Public Key
        BIO* bio = BIO_new(BIO_s_mem());
        PEM_write_bio_EC_PUBKEY(bio, key);
        BUF_MEM* mem;
        BIO_get_mem_ptr(bio, &mem);
        publicID = std::string(mem->data, mem->length);
        BIO_free(bio);

        // Get Private Key
        bio = BIO_new(BIO_s_mem());
        PEM_write_bio_ECPrivateKey(bio, key, NULL, NULL, 0, NULL, NULL);
        BIO_get_mem_ptr(bio, &mem);
        privateKey = std::string(mem->data, mem->length);
        BIO_free(bio);

        EC_KEY_free(key);
    }

    bool Wallet::ValidatePrivateKey(const std::string& privateKey, const std::string& publicID) {
        std::string testHash = "0000abc1e11b8d37c1e1232a2ea6d290cddb0c678058c37aa766f813cbbb366e"; 

        if (privateKey.empty() || publicID.empty())
            return false;

        std::string sig = CreateSignature(publicID, privateKey, testHash);
        return ValidateSignature(publicID, testHash, sig);
    }

    bool Wallet::ValidateSignature(const std::string& publicID, const std::string& datahash, const std::string& datasig) {
        EC_KEY* key = CreateKey(publicID);
        if (!key) return false;

        std::vector<unsigned char> hashBytes = Base64Decode(datahash);
        std::vector<unsigned char> sigBytes = Base64Decode(datasig);

        int valid = ECDSA_verify(0, hashBytes.data(), hashBytes.size(), sigBytes.data(), sigBytes.size(), key);
        EC_KEY_free(key);
        return valid == 1;
    }

    std::string Wallet::CreateSignature(const std::string& publicID, const std::string& privateKey, const std::string& datahash) {
        EC_KEY* key = CreateKey(publicID, privateKey);
        if (!key) return "null";

        std::vector<unsigned char> hashBytes = Base64Decode(datahash);
        unsigned int sigLen = ECDSA_size(key);
        std::vector<unsigned char> sig(sigLen);

        if (ECDSA_sign(0, hashBytes.data(), hashBytes.size(), sig.data(), &sigLen, key) == 0) {
            EC_KEY_free(key);
            return "null";
        }

        EC_KEY_free(key);
        sig.resize(sigLen);
        return Base64Encode(sig);
    }

    EC_KEY* Wallet::CreateKey(const std::string& publicID, const std::string& privateKey) {
        BIO* bio = BIO_new_mem_buf(publicID.data(), publicID.size());
        EC_KEY* key = PEM_read_bio_EC_PUBKEY(bio, NULL, NULL, NULL);
        BIO_free(bio);

        if (!privateKey.empty()) {
            bio = BIO_new_mem_buf(privateKey.data(), privateKey.size());
            EC_KEY* privKey = PEM_read_bio_ECPrivateKey(bio, NULL, NULL, NULL);
            BIO_free(bio);
            if (privKey) return privKey;
        }

        return key;
    }

    std::string Wallet::Base64Encode(const std::vector<unsigned char>& data) {
        BIO* bio, * b64;
        BUF_MEM* bufferPtr;
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);
        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        BIO_write(bio, data.data(), data.size());
        BIO_flush(bio);
        BIO_get_mem_ptr(bio, &bufferPtr);
        std::string encoded(bufferPtr->data, bufferPtr->length);
        BIO_free_all(bio);
        return encoded;
    }

    std::vector<unsigned char> Wallet::Base64Decode(const std::string& encoded) {
        BIO* bio, * b64;
        int decodeLen = encoded.size() * 3 / 4;
        std::vector<unsigned char> buffer(decodeLen);

        bio = BIO_new_mem_buf(encoded.data(), encoded.size());
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_push(b64, bio);
        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        int length = BIO_read(bio, buffer.data(), buffer.size());
        BIO_free_all(bio);

        buffer.resize(length);
        return buffer;
    }

} // namespace BlockchainAssignment::Wallet
