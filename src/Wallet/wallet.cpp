#include "wallet.h"
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/core_names.h>
#include <openssl/param_build.h>

#include <iostream>

namespace BlockchainAssignment {
    namespace Wallet {
        
        // Base64 encoding function
        std::string Wallet::base64_encode(const std::vector<unsigned char>& input) {
            BIO *bio, *b64;
            BUF_MEM *bufferPtr;

            b64 = BIO_new(BIO_f_base64());
            bio = BIO_new(BIO_s_mem());
            bio = BIO_push(b64, bio);

            BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
            BIO_write(bio, input.data(), input.size());
            BIO_flush(bio);
            BIO_get_mem_ptr(bio, &bufferPtr);
            
            std::string result(bufferPtr->data, bufferPtr->length);
            BIO_free_all(bio);

            return result;
        }

        // Base64 decoding function
        std::vector<unsigned char> Wallet::base64_decode(const std::string& input) {
            BIO *bio = nullptr, *b64 = nullptr;
            std::vector<unsigned char> result;

            // Quick check for empty or obviously invalid input
            if (input.empty() || input == "null") {
                return result;
            }

            result.resize(input.size());

            b64 = BIO_new(BIO_f_base64());
            bio = BIO_new_mem_buf(input.c_str(), input.length());
            bio = BIO_push(b64, bio);

            BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
            int decoded_size = BIO_read(bio, result.data(), input.size());

            if (decoded_size <= 0) {
                result.clear();
            } else {
                result.resize(decoded_size);
            }

            BIO_free_all(bio);
            return result;
        }

        // Constructor that generates a new wallet
        Wallet::Wallet(std::string& privateKey) {
            privateKey = "";
            
            EVP_PKEY* pkey = EVP_EC_gen("prime256v1");
            if (!pkey) {
                std::cerr << "Failed to generate EC key pair" << std::endl;
                return;
            }

            // Extract public key
            size_t pubkey_len;
            if (EVP_PKEY_get_octet_string_param(pkey, OSSL_PKEY_PARAM_PUB_KEY, 
                                              nullptr, 0, &pubkey_len) != 1) {
                std::cerr << "Failed to get public key length" << std::endl;
                EVP_PKEY_free(pkey);
                return;
            }
            
            std::vector<unsigned char> pub_key_bytes(pubkey_len);
            if (EVP_PKEY_get_octet_string_param(pkey, OSSL_PKEY_PARAM_PUB_KEY, 
                                              pub_key_bytes.data(), pubkey_len, 
                                              &pubkey_len) != 1) {
                std::cerr << "Failed to get public key bytes" << std::endl;
                EVP_PKEY_free(pkey);
                return;
            }

            // Extract private key
            BIGNUM* priv_bn = nullptr;
            if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_PRIV_KEY, &priv_bn) != 1) {
                std::cerr << "Failed to get private key" << std::endl;
                EVP_PKEY_free(pkey);
                return;
            }
            
            std::vector<unsigned char> priv_key_bytes(BN_num_bytes(priv_bn));
            BN_bn2bin(priv_bn, priv_key_bytes.data());
            BN_free(priv_bn);

            publicID = base64_encode(pub_key_bytes);
            privateKey = base64_encode(priv_key_bytes);
            
            EVP_PKEY_free(pkey);
        }

        // Validate if a private key matches a public ID
        bool Wallet::ValidatePrivateKey(const std::string& privateKey, const std::string& publicID) {
            std::string testHash = "0000abc1e11b8d37c1e1232a2ea6d290cddb0c678058c37aa766f813cbbb366e";

            if (privateKey.length() != 44 || publicID.length() != 88) return false;

            std::string sig = CreateSignature(publicID, privateKey, testHash);
            return ValidateSignature(publicID, testHash, sig);
        }

        // Validates if a signature is legitimate
        bool Wallet::ValidateSignature(std::string publicID, const std::string& datahash, const std::string& datasig) {
            // Handle special case for mining rewards
            if (publicID == "Mine Rewards")
                publicID = "BFlEn2Nq/7ux8k5WqYsPm7+OEJiwXtlJpDva1JXPqRHjafByB28DUF50Mz1FBTTG94Js2I2H3j3Z8//sr8KEp2E=";

            if (publicID.length() != 88 || datasig == "null") return false;

            EVP_PKEY* pkey = createKey(publicID);
            if (!pkey) return false;

            std::vector<unsigned char> datahash_bytes = base64_decode(datahash);
            std::vector<unsigned char> signature_bytes = base64_decode(datasig);

            EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
            if (!ctx) {
                EVP_PKEY_free(pkey);
                return false;
            }
            if (EVP_PKEY_verify_init(ctx) <= 0) {
                EVP_PKEY_CTX_free(ctx);
                EVP_PKEY_free(pkey);
                return false;
            }

            int verify_result = EVP_PKEY_verify(
                ctx,
                signature_bytes.data(), signature_bytes.size(),
                datahash_bytes.data(), datahash_bytes.size()
            );

            EVP_PKEY_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            return verify_result == 1;
        }

        // Creates a signature using the private key
        std::string Wallet::CreateSignature(const std::string& publicID, 
                                            const std::string& privateKey, 
                                            const std::string& datahash) {
            EVP_PKEY* pkey = createKey(publicID, privateKey);
            if (!pkey)
                return "null";
            
            std::vector<unsigned char> digest_bytes = base64_decode(datahash);
            
            EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
            if (!ctx) {
                EVP_PKEY_free(pkey);
                return "null";
            }
            
            if (EVP_PKEY_sign_init(ctx) <= 0) {
                EVP_PKEY_CTX_free(ctx);
                EVP_PKEY_free(pkey);
                return "null";
            }
            
            size_t sig_len = 0;
            if (EVP_PKEY_sign(ctx, nullptr, &sig_len, digest_bytes.data(), digest_bytes.size()) <= 0) {
                EVP_PKEY_CTX_free(ctx);
                EVP_PKEY_free(pkey);
                return "null";
            }
            
            std::vector<unsigned char> signature(sig_len);
            if (EVP_PKEY_sign(ctx, signature.data(), &sig_len, digest_bytes.data(), digest_bytes.size()) <= 0) {
                EVP_PKEY_CTX_free(ctx);
                EVP_PKEY_free(pkey);
                return "null";
            }
            signature.resize(sig_len);
            
            EVP_PKEY_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            return base64_encode(signature);
        }

        EVP_PKEY* Wallet::createKey(const std::string& publicID, const std::string& privateKey) {
            EVP_PKEY* pkey = nullptr;
            EVP_PKEY_CTX* ctx = nullptr;
            OSSL_PARAM_BLD* param_bld = nullptr;
            OSSL_PARAM* params = nullptr;
            BIGNUM* priv_bn = nullptr;

            try {
                std::string pub_id = publicID;
                std::string priv_key = privateKey;

                // Default key pair for "Mine Rewards"
                if (publicID == "Mine Rewards" && privateKey.empty()) {
                    pub_id = "BFlEn2Nq/7ux8k5WqYsPm7+OEJiwXtlJpDva1JXPqRHjafByB28DUF50Mz1FBTTG94Js2I2H3j3Z8//sr8KEp2E=";
                    priv_key = "D+vH2vRdw/mgnxiNCA0H40erJpzVV5gq0eCGx2tDQoA=";
                }

                std::vector<unsigned char> pub_bytes = base64_decode(pub_id);

                // initialize parameter builder
                param_bld = OSSL_PARAM_BLD_new();
                if (!param_bld) {
                    std::cerr << "Failed to create OSSL_PARAM_BLD" << std::endl;
                    goto cleanup;
                }

                // Set the curve name (NIST P-256)
                if (!OSSL_PARAM_BLD_push_utf8_string(param_bld, OSSL_PKEY_PARAM_GROUP_NAME, "prime256v1", 0)) {
                    std::cerr << "Failed to set group name" << std::endl;
                    goto cleanup;
                }

                // Set the public key as an octet string
                // Note: pub_bytes should be in uncompressed format (65 bytes: 0x04 || X || Y)
                if (!OSSL_PARAM_BLD_push_octet_string(param_bld, OSSL_PKEY_PARAM_PUB_KEY, pub_bytes.data(), pub_bytes.size())) {
                    std::cerr << "Failed to set public key" << std::endl;
                    goto cleanup;
                }

                // Set the private key if provided
                if (!priv_key.empty()) {
                    std::vector<unsigned char> priv_bytes = base64_decode(priv_key);
                    priv_bn = BN_bin2bn(priv_bytes.data(), priv_bytes.size(), nullptr);
                    if (!priv_bn || !OSSL_PARAM_BLD_push_BN(param_bld, OSSL_PKEY_PARAM_PRIV_KEY, priv_bn)) {
                        std::cerr << "Failed to set private key" << std::endl;
                        goto cleanup;
                    }
                }

                // Build the parameters
                params = OSSL_PARAM_BLD_to_param(param_bld);
                if (!params) {
                    std::cerr << "Failed to build parameters" << std::endl;
                    goto cleanup;
                }

                // Create the EVP_PKEY using the parameters
                ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
                if (!ctx || EVP_PKEY_fromdata_init(ctx) <= 0) {
                    std::cerr << "Failed to initialize EVP_PKEY context" << std::endl;
                    goto cleanup;
                }

                if (EVP_PKEY_fromdata(ctx, &pkey, EVP_PKEY_KEYPAIR, params) <= 0) {
                    std::cerr << "Failed to create EVP_PKEY from data" << std::endl;
                    goto cleanup;
                }

                // Cleanup and return
                BN_free(priv_bn);
                OSSL_PARAM_free(params);
                OSSL_PARAM_BLD_free(param_bld);
                EVP_PKEY_CTX_free(ctx);
                return pkey;

            cleanup:
                if (pkey) EVP_PKEY_free(pkey);
                BN_free(priv_bn);
                OSSL_PARAM_free(params);
                OSSL_PARAM_BLD_free(param_bld);
                EVP_PKEY_CTX_free(ctx);
                return nullptr;
            }
            catch (const std::exception& e) {
                std::cerr << "Error in createKey: " << e.what() << std::endl;
                EVP_PKEY_free(pkey);
                BN_free(priv_bn);
                OSSL_PARAM_free(params);
                OSSL_PARAM_BLD_free(param_bld);
                EVP_PKEY_CTX_free(ctx);
                return nullptr;
            }
        }
    }
}