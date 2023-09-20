#include "elementGroupModel.h"
#include <QHash>

ElementGroupModel::ElementGroupModel(QObject *parent)
  : m_groups({})
{
}

void
ElementGroupModel::insert(WindowElementGroup *group)
{
    m_groups.push_back(group);
    connect(group, &WindowElementGroup::deleteSelf, this, &ElementGroupModel::remove);
    Q_EMIT groupsChanged();
}

void
ElementGroupModel::remove(WindowElementGroup *group)
{
    m_groups.removeOne(group);
    Q_EMIT groupsChanged();
}
