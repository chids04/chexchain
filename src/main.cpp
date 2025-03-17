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

    auto b_app = engine.singletonInstance<BlockchainApp*>("blockchain", "BlockchainApp");
    b_app->init();

    return app.exec();
}
