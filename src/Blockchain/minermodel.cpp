#include "minermodel.h"

MinerModel::MinerModel(QObject* parent)
    : QAbstractListModel(parent)
{}

int MinerModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_miners.size());
}

QVariant MinerModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_miners.size()))
        return {};

    const Miner& miner = m_miners[index.row()];
    switch (role) {
        case NameRole:
            return miner.name;
        case BlocksMinedRole:
            return miner.blocksMined;
        default:
            return {};
    }
}

QHash<int, QByteArray> MinerModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[BlocksMinedRole] = "blocksMined";
    return roles;
}

void MinerModel::addMiner(const Miner& miner) {
    beginInsertRows(QModelIndex(), m_miners.size(), m_miners.size());
    m_miners.push_back(miner);
    endInsertRows();
}

void MinerModel::clear() {
    beginResetModel();
    m_miners.clear();
    endResetModel();
}

const std::vector<Miner>& MinerModel::miners() const {
    return m_miners;
}