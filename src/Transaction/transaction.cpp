#include "transaction.h"
#include "utility.h"
#include "hashtools.h"
#include "wallet.h"

#include <print>
#include <stdexcept>

using namespace BlockchainAssignment;

int nonce = 0;

Transaction::Transaction() {}

Transaction::Transaction(const std::string &sender, const std::string &receiver, const std::string &privKey, 
    double amount, double fee)
    : sender(sender), receiver(receiver), timestamp(Utility::genTimeStamp()), amount(amount), fee(fee)
{

    //incremented nonce added to add ensure no two transactions can hash to the same val
    tx_nonce = nonce;
    nonce++;

    std::string hash_data = sender + receiver + std::to_string(timestamp) + 
                            std::to_string(amount) + std::to_string(fee) + std::to_string(nonce);

    hash = HashCode::genSHA256(hash_data);
    sig = Wallet::Wallet::CreateSignature(sender, privKey, hash);
    
    std::print("transaction sig {}", sig);
    
    //add error checking if failed to create a transaction
    if(sig == "null"){
        //throw exception and revert nonce to prevent int overflow from repeatedly sending false transactions
        nonce--;
        throw std::runtime_error("Error in creating transaction, sender and/or receiver keys may be invalid");
    }
}

std::string Transaction::printTransaction()
{
    std::string time_str = Utility::printTime(timestamp);

    std::string msg = 
    "Transaction Hash: " + hash + "\n" +
    "Digital Signature: " + sig + "\n" + 
    "Timestamp: " + time_str + "\n" + 
    "Transferred: " + std::to_string(amount) + " AssignmentCoin" + "\n" +
    "Fees: " + std::to_string(fee) + "\n" + 
    "Sender Address: " + sender + "\n"  + 
    "Receiver Address: " + receiver;

    return msg;
}

