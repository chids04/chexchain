#pragma once

#include <string>


class Transaction{
    

public:
    Transaction(const std::string &sender, const std::string &receiver,
        const std::string &privKey, double amount, double fee);

    std::string printTransaction();
    

    std::string hash;
    std::string sig;
    std::string sender;
    std::string receiver;

    long long timestamp;
    double amount;
    double fee;
};