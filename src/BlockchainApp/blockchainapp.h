#include "blockchain.h"
#include "transaction.h"

#include <iostream>
#include <QObject>
#include <QQmlEngine>
#include <QString>

class BlockchainApp : public QObject{

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    void init();

signals:
    void printMsg(const QString &msg);
    void showPubKey(const QString &key);
    void showPrivKey(const QString &key);

public slots:
    void getBlockInfo(int index);
    void readAllBlocks();
    void generateBlock();
    void generateWallet();
    void validateWallet(const QString &priv, const QString &pub);
    void createTransaction(const QString &sender, const QString &privKey,
        const QString &receiver, double amount, double fee);
    void validateTransaction();
    void printPendingTransactions();

private:
    Blockchain blockchain;
    Transaction transaction;
};
