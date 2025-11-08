#pragma once

#include <string>

extern int nonce;

class Transaction{


public:
    Transaction();
    Transaction(const std::string &sender, const std::string &receiver,
        const std::string &privKey, double amount, double fee);

    std::string printTransaction() const;

    std::string hash;
    std::string sig;
    std::string sender;
    std::string receiver;

    long long timestamp;
    double amount;
    double fee;
    int tx_nonce;
};
