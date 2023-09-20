#include "elementGroupModel.h"
#include <QHash>

ElementGroupModel::ElementGroupModel(QObject *parent)
  : m_groups({})
{
}

void
ElementGroupModel::insert(WindowElementGroup *group)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_groups.push_back(group);
    endInsertRows();
}

QVariant
ElementGroupModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Display:
        return m_groups[index.row()]->id();
    case Icon:
        return m_groups[index.row()]->icon();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray>
ElementGroupModel::roleNames() const
{
    static const QHash<int, QByteArray> roles{{Display, "Name"}, {Icon, "Icon"}};
    return roles;
}

int
ElementGroupModel::rowCount(const QModelIndex &) const
{
    return m_groups.count();
}
