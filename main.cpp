#include <iostream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qDebug>

#include "wallet.h"
#include "blockchainapp.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("blockchain", "Main");

    BlockchainAssignment::Wallet::Wallet myWallet;
    std::cout << "Private Key: " << myWallet.privateKey << "\n";

    std::string hash = "0000abc1e11b8d37c1e1232a2ea6d290cddb0c678058c37aa766f813cbbb366e";
    std::string signature = BlockchainAssignment::Wallet::Wallet::CreateSignature(myWallet.publicID, myWallet.privateKey, hash);

    bool isValid = BlockchainAssignment::Wallet::Wallet::ValidateSignature(myWallet.publicID, hash, signature);
    std::cout << "Signature Valid: " << (isValid ? "YES" : "NO") << "\n";

    auto b_app = engine.singletonInstance<BlockchainApp*>("blockchain", "BlockchainApp");
    b_app->init();

    return app.exec();
}
