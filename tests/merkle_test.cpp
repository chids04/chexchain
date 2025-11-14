#include <cstddef>
#include <gtest/gtest.h>
#include "block.h"
#include "transaction.h"
#include "hashtools.h"
#include <vector>
#include <memory>

#include "wallet.h"

class MerkleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // create test transactions

        auto tx1 = Transaction("Alice", "Bob", "priv1", 5.0, 0.1);
        auto tx2 = Transaction("Charlie", "Dave", "priv2", 3.0, 0.05);
        auto tx3 = Transaction("Eve", "Frank", "priv3", 7.0, 0.2);
        auto tx4 = Transaction("Grace", "Henry", "priv4", 2.0, 0.08);

        // store hashes for manual verification
        hash1 = tx1.hash;
        hash2 = tx2.hash;
        hash3 = tx3.hash;
        hash4 = tx4.hash;
    }

    Transaction tx1, tx2, tx3, tx4;
    std::string hash1, hash2, hash3, hash4;
};

TEST_F(MerkleTest, EmptyTransactionsReturnsEmptyRoot) {
    std::vector<Transaction> empty_transactions;
    std::string root = Block::computeMerkleRoot(empty_transactions);
    EXPECT_TRUE(root.empty());
}

TEST_F(MerkleTest, OddTransactionsDuplciate) {
    std::vector<Transaction> tx;

    tx.push_back(tx1);
    tx.push_back(tx2);
    tx.push_back(tx3);

    auto merkRoot = Block::computeMerkleRoot(tx);
    //       h(12, 33)
    //         |
    //  h(1,2)   h(3,3)
    //    |        |
    // tx1 tx2 tx3 tx3

    auto h12 = BlockchainAssignment::HashCode::CombineHash(tx[0].hash, tx[1].hash);
    auto h33 = BlockchainAssignment::HashCode::CombineHash(tx[2].hash, tx[2].hash);

    auto root = BlockchainAssignment::HashCode::CombineHash(h12, h33);

    ASSERT_EQ(merkRoot, root);

}
