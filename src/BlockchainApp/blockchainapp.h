#include "blockchain.h"

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

public slots:
    void getBlockInfo(int index);

private:
    Blockchain blockchain;
};