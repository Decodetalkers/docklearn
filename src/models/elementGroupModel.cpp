#include "elementGroupModel.h"
#include "backgrounds/x11/XorgBackground.h"
#include <QDebug>
#include <QHash>

ElementGroupModel::ElementGroupModel(QObject *parent)
  : m_groups({})
{
    m_xorgBackground = new XorgBackground(this);
    connect(m_xorgBackground, &XorgBackground::windowGenerated, this, &ElementGroupModel::insert);
    m_xorgBackground->start();
}

ElementGroupModel::~ElementGroupModel()
{
    m_xorgBackground->stoploop();
    m_xorgBackground->quit();
    m_xorgBackground->wait();
}

void
ElementGroupModel::insert(WindowElement *window)
{
    QVector<WindowElementGroup *>::iterator it =
      std::find_if(m_groups.begin(), m_groups.end(), [window](WindowElementGroup *g) {
          return g->id() == window->id();
      });
    if (it != m_groups.end()) {
        (*it)->appendWindowElement(window);
        Q_EMIT groupsChanged();
        return;
    }
    WindowElementGroup *group = new WindowElementGroup(false, window->id(), this);
    group->appendWindowElement(window);
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
