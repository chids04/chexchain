#include "transaction.h"
#include "utility.h"
#include "hashtools.h"
#include "wallet.h"

using namespace BlockchainAssignment;

Transaction::Transaction(const std::string &sender, const std::string &receiver, const std::string &privKey, 
    double amount, double fee)
    : sender(sender), receiver(receiver), timestamp(Utility::genTimeStamp()), amount(amount), fee(fee)
{
    std::string hash_data = sender + receiver + std::to_string(timestamp) + 
                            std::to_string(amount) + std::to_string(fee);

    hash = HashCode::genSHA256(hash_data);
    sig = Wallet::Wallet::CreateSignature(sender, privKey, hash);
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
