#include "wallet.h"
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <iostream>
#include <algorithm>

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
            BIO *bio, *b64;
            
            // Create buffer large enough to hold decoded data
            std::vector<unsigned char> result(input.size());
            
            b64 = BIO_new(BIO_f_base64());
            bio = BIO_new_mem_buf(input.c_str(), input.length());
            bio = BIO_push(b64, bio);
            
            BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
            int decoded_size = BIO_read(bio, result.data(), input.size());
            
            // Resize to actual decoded size
            result.resize(decoded_size);
            
            BIO_free_all(bio);
            return result;
        }

        // Constructor that generates a new wallet
        Wallet::Wallet(std::string& privateKey) {
            privateKey = "";
            
            // Initialize OpenSSL
            OpenSSL_add_all_algorithms();
            ERR_load_crypto_strings();
            
            // Create a new EC key (equivalent to ECDsaP256)
            EC_KEY* eckey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
            if (!eckey) {
                std::cerr << "Failed to create EC key" << std::endl;
                return;
            }
            
            // Generate key pair
            if (EC_KEY_generate_key(eckey) != 1) {
                std::cerr << "Failed to generate EC key pair" << std::endl;
                EC_KEY_free(eckey);
                return;
            }
            
            // Get public key
            const EC_POINT* pub_key = EC_KEY_get0_public_key(eckey);
            if (!pub_key) {
                std::cerr << "Failed to get public key" << std::endl;
                EC_KEY_free(eckey);
                return;
            }
            
            // Get private key
            const BIGNUM* priv_key = EC_KEY_get0_private_key(eckey);
            if (!priv_key) {
                std::cerr << "Failed to get private key" << std::endl;
                EC_KEY_free(eckey);
                return;
            }
            
            // Convert public key to bytes
            EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
            size_t pub_key_size = EC_POINT_point2oct(group, pub_key, POINT_CONVERSION_UNCOMPRESSED, NULL, 0, NULL);
            std::vector<unsigned char> pub_key_bytes(pub_key_size);
            EC_POINT_point2oct(group, pub_key, POINT_CONVERSION_UNCOMPRESSED, pub_key_bytes.data(), pub_key_bytes.size(), NULL);
            
            // Convert private key to bytes
            size_t priv_key_size = BN_num_bytes(priv_key);
            std::vector<unsigned char> priv_key_bytes(priv_key_size);
            BN_bn2bin(priv_key, priv_key_bytes.data());
            
            // Store keys as base64 strings
            publicID = base64_encode(pub_key_bytes);
            privateKey = base64_encode(priv_key_bytes);
            
            // Clean up
            EC_GROUP_free(group);
            EC_KEY_free(eckey);
        }

        // Validate if a private key matches a public ID
        bool Wallet::ValidatePrivateKey(const std::string& privateKey, const std::string& publicID) {
            // Random string used to create a verification signature
            std::string testHash = "0000abc1e11b8d37c1e1232a2ea6d290cddb0c678058c37aa766f813cbbb366e";

            if (privateKey.length() != 44 || publicID.length() != 88)
                return false;

            std::string sig = CreateSignature(publicID, privateKey, testHash);

            return ValidateSignature(publicID, testHash, sig);
        }

        // Validates if a signature is legitimate
        bool Wallet::ValidateSignature(std::string publicID, const std::string& datahash, const std::string& datasig) {
            // Handle special case for "Mine Rewards"
            if (publicID == "Mine Rewards")
                publicID = "QfF3+9GgTxyGLvb+ScOAI6nJxBh8IyZbeD0r6BJBMyabZmyuP82yrSLKMq/F05OG0VZ4gg63uHFZUKzCu3wZuA==";
            
            if (publicID.length() != 88 || datasig == "null")
                return false;
            
            EC_KEY* key = createKey(publicID);
            
            if (!key)
                return false;
            
            std::vector<unsigned char> datahash_bytes = base64_decode(datahash);
            std::vector<unsigned char> signature_bytes = base64_decode(datasig);
            
            // Verify signature
            int verify_status = ECDSA_verify(0, datahash_bytes.data(), datahash_bytes.size(), 
                                            signature_bytes.data(), signature_bytes.size(), key);
            
            EC_KEY_free(key);
            return (verify_status == 1);
        }

        // Creates a signature using the private key
        std::string Wallet::CreateSignature(const std::string& publicID, const std::string& privateKey, const std::string& datahash) {
            EC_KEY* key = createKey(publicID, privateKey);
            
            if (!key)
                return "null";
            
            std::vector<unsigned char> datahash_bytes = base64_decode(datahash);
            
            // Create signature buffer
            std::vector<unsigned char> signature(ECDSA_size(key));
            unsigned int sig_len = 0;
            
            // Sign the data
            int sign_status = ECDSA_sign(0, datahash_bytes.data(), datahash_bytes.size(), 
                                       signature.data(), &sig_len, key);
            
            EC_KEY_free(key);
            
            if (sign_status != 1)
                return "null";
            
            signature.resize(sig_len);
            return base64_encode(signature);
        }

        // Creates an EC_KEY from a public ID and optional private key
        EC_KEY* Wallet::createKey(const std::string& publicID, const std::string& privateKey) {
            try {
                std::string pub_id = publicID;
                std::string priv_key = privateKey;
                
                // Handle special case for "Mine Rewards"
                if (publicID == "Mine Rewards" && privateKey.empty()) {
                    pub_id = "QfF3+9GgTxyGLvb+ScOAI6nJxBh8IyZbeD0r6BJBMyabZmyuP82yrSLKMq/F05OG0VZ4gg63uHFZUKzCu3wZuA==";
                    priv_key = "mkT1Iu3YF4NSruHBptVytyDkNcxwemrkclndJH0+73o=";
                }
                
                EC_KEY* key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
                if (!key) {
                    std::cerr << "Failed to create new EC key" << std::endl;
                    return nullptr;
                }
                
                // Decode public key
                std::vector<unsigned char> pub_bytes = base64_decode(pub_id);
                
                // Convert bytes to EC_POINT
                EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
                EC_POINT* pub_point = EC_POINT_new(group);
                
                if (!EC_POINT_oct2point(group, pub_point, pub_bytes.data(), pub_bytes.size(), nullptr)) {
                    std::cerr << "Failed to convert public key bytes to EC_POINT" << std::endl;
                    EC_POINT_free(pub_point);
                    EC_GROUP_free(group);
                    EC_KEY_free(key);
                    return nullptr;
                }
                
                // Set public key
                if (EC_KEY_set_public_key(key, pub_point) != 1) {
                    std::cerr << "Failed to set public key" << std::endl;
                    EC_POINT_free(pub_point);
                    EC_GROUP_free(group);
                    EC_KEY_free(key);
                    return nullptr;
                }
                
                // If private key is provided, set it
                if (!priv_key.empty()) {
                    std::vector<unsigned char> priv_bytes = base64_decode(priv_key);
                    BIGNUM* priv_bn = BN_bin2bn(priv_bytes.data(), priv_bytes.size(), nullptr);
                    
                    if (!priv_bn) {
                        std::cerr << "Failed to convert private key bytes to BIGNUM" << std::endl;
                        EC_POINT_free(pub_point);
                        EC_GROUP_free(group);
                        EC_KEY_free(key);
                        return nullptr;
                    }
                    
                    if (EC_KEY_set_private_key(key, priv_bn) != 1) {
                        std::cerr << "Failed to set private key" << std::endl;
                        BN_free(priv_bn);
                        EC_POINT_free(pub_point);
                        EC_GROUP_free(group);
                        EC_KEY_free(key);
                        return nullptr;
                    }
                    
                    BN_free(priv_bn);
                }
                
                EC_POINT_free(pub_point);
                EC_GROUP_free(group);
                
                return key;
            }
            catch (const std::exception& e) {
                std::cerr << "Error in createKey: " << e.what() << std::endl;
                return nullptr;
            }
        }
    }
}