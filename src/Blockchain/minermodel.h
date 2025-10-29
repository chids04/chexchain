#pragma once

#include <QAbstractListModel>
#include <QString>
#include <vector>

// Simple Miner struct for demonstration
struct Miner {
    QString name;
    int blocksMined;
};

class MinerModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum MinerRoles {
        NameRole = Qt::UserRole + 1,
        BlocksMinedRole
    };

    explicit MinerModel(QObject* parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // Role names for QML
    QHash<int, QByteArray> roleNames() const override;

    // Miner management
    void addMiner(const Miner& miner);
    void clear();

    const std::vector<Miner>& miners() const;

private:
    std::vector<Miner> m_miners;
};