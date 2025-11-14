#include "signer.h"
#include <openssl/ecdsa.h>
#include <print>

Signer::Signer() : priv_key(EVP_EC_gen("P-256")) {

    EVP_MD* pub_digest;
    EVP_MD_CTX *digest_ctx;
    unsigned char* digest_val;

    try {
        BIGNUM *x = NULL;
        BIGNUM *y = NULL;

        if(EVP_PKEY_get_bn_param(priv_key, OSSL_PKEY_PARAM_EC_PUB_X, &x) != 1) {
            fprintf(stderr, "error generating public key: invalid ec x param");

            throw "invalid ec x param";
            //goto cleanup;
        }
        if(EVP_PKEY_get_bn_param(priv_key, OSSL_PKEY_PARAM_EC_PUB_Y, &y) != 1) {
            fprintf(stderr, "error generating public key: invalid ec y param");

            throw "invalid ec y param";
        }

        unsigned char xy[64];
        int x_len = BN_bn2binpad(x, xy, 32);
        int y_len = BN_bn2binpad(x, &xy[32], 32);

        if(x_len < 0 || y_len < 0) {
            fprintf(stderr, "invalid x and/or y values from ECDSA point curve");

            throw "invalid ecdsa curve points";
        }

        BN_free(x);
        BN_free(y);

        // now we can hash this
        // and take last 20 bytes

        EVP_MD* pub_digest = EVP_MD_fetch(NULL, HASH_NAME.c_str(), NULL);

        unsigned int digest_len = EVP_MD_get_size(pub_digest);
        if(digest_len <= 0) {
            fprintf(stderr, "failed to create digest len for pub key");

            throw "failed to create digest";
        }

        digest_val = static_cast<unsigned char*>(OPENSSL_malloc(digest_len));

        if(digest_val == NULL) {
            fprintf(stderr, "failed to create digest value for pub key");

            throw "failed to allocate memory for digest";

        }

        digest_ctx = EVP_MD_CTX_new();
        if(digest_ctx == NULL) {
            fprintf(stderr, "failed to create digest context for pub key");

            throw "failed to create digest context";
        }

        if(EVP_DigestInit(digest_ctx, pub_digest) != 1) {
            fprintf(stderr, "failed to init digest for pub key");

            throw "failed to init digest for pub key";
        }

        if(EVP_DigestUpdate(digest_ctx, xy, sizeof(xy)) != 1) {
            fprintf(stderr, "failed to update digest for pub key");

            throw "failed to update digest";
        }

        if(EVP_DigestFinal(digest_ctx, digest_val, &digest_len) != 1) {
            fprintf(stderr, "failed to finalise digest for pub key");

            throw "failed to init digest";
        }

        // unsigned char *pub = malloc(sizeof(unsigned char) * 20);


        std::memcpy(pub_key.data(), digest_val + (size_t)digest_len - 20, 20);

        printf("full shortened generated pubkey (len %d): ", 20);
        for(unsigned int i=0; i<20; i++) {
            printf("%02x", pub_key[i]);
        }
        printf("\n");

        printf("full generated pubkey (len %d): ", digest_len);
        for(unsigned int i=0; i<digest_len; i++) {
            printf("%02x", digest_val[i]);
        }


        EVP_MD_CTX_free(digest_ctx);
        EVP_MD_free(pub_digest);
        OPENSSL_free(digest_val);


    } catch(const char *ex) {
        std::print("failed to gen pub key: {}", ex);

        EVP_MD_CTX_free(digest_ctx);
        EVP_MD_free(pub_digest);
        OPENSSL_free(digest_val);

        // this is terrible but idk what else to do, will come back
        exit(1);

    }
}

std::vector<unsigned char> Signer::sign(std::vector<unsigned char> data) {
    EVP_MD_CTX * sign_context;

    try {

        sign_context = EVP_MD_CTX_new();
        if(sign_context == NULL) {

            throw "EVP_MD_CTX_NEW failed \n";
        }
        if (!EVP_DigestSignInit_ex(sign_context, NULL, HASH_NAME.c_str(), NULL,NULL, priv_key, NULL)) {
            throw "EVP_DigestSignInit_ex failed.\n";
        }

        if(!EVP_DigestSignUpdate(sign_context, data.data(), data.size())) {
            throw "EVP_DigestSignUpdate failed, \n";
        }

        size_t sig_len;

        if(!EVP_DigestSignFinal(sign_context, NULL, &sig_len)) {
            throw "sign() EVP_DigestSignFinal failed to get signature length\n";
        }

        if(sig_len <= 0) {
            throw "EVP_DigestSignFinal retunred invalid signature length\n";
        }


        std::vector<unsigned char> sig_bytes;
        sig_bytes.resize(sig_len);


        if(!EVP_DigestSignFinal(sign_context, sig_bytes.data(), &sig_len)) {
            throw "sign() EVP_DigestSignFinal failed to finalise signature\n";
        }

        // extract r and s components from the ecdsa sig
        ECDSA_SIG *obj;
        const BIGNUM *r, *s;

        const unsigned char* raw_sig = sig_bytes.data();

        /* Load a signature into the ECDSA_SIG object */
        obj = d2i_ECDSA_SIG(NULL, &raw_sig, static_cast<long>(sig_bytes.size()));
        if (obj == NULL){
            throw "failed to create raw ecdsa signature";
        }
            /* error */

        r = ECDSA_SIG_get0_r(obj);
        s = ECDSA_SIG_get0_s(obj);
        if (r == NULL || s == NULL){
            throw "failed to extract r and s component of ecdsa signature";
        }

        return sig_bytes;

    }
    catch(const char *ex) {

        std::println("error signing data: {}", ex);
        EVP_MD_CTX_free(sign_context);
    }
}
