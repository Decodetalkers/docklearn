#include "windowelement.h"
#include <algorithm>

WindowElement::WindowElement(const QString &id, QObject *parent)
  : m_id(id)
  , m_icon(QString())
{
}

void
WindowElement::deleteSelf()
{
    Q_EMIT selfDestory(this);
    this->deleteLater();
}

void
WindowElement::requestActiveChanged()
{
    // TODO:
    Q_EMIT activeChanged();
}

void
WindowElement::setIcon(const QString &icon)
{
    m_icon = icon;
    Q_EMIT iconChanged();
    Q_EMIT requestGroupIconChanged(m_icon);
}

WindowElementGroup::WindowElementGroup(bool isPin, const QString &id, QObject *parent)
  : m_isPin(isPin)
  , m_id(id)
  , m_elements({})
{
}

void
WindowElementGroup::appendWindowElement(WindowElement *element)
{
    connect(element, &WindowElement::selfDestory, this, &WindowElementGroup::removeWindowElement);
    connect(element, &WindowElement::requestGroupIconChanged, this, &WindowElementGroup::setIcon);
    m_icon = element->icon();
    m_elements.push_back(element);
    Q_EMIT elementsChanged();
}

void
WindowElementGroup::removeWindowElement(WindowElement *element)
{
    m_elements.removeOne(element);
    if (m_elements.isEmpty() || !m_isPin) {
        Q_EMIT deleteSelf(this);
        return;
    }
    Q_EMIT elementsChanged();
}

void
WindowElementGroup::setIcon(const QString &icon)
{
    m_icon = icon;
    Q_EMIT iconChanged();
}

void
WindowElementGroup::requestActiveChanged()
{
    QVector<WindowElement *>::iterator it = std::find_if(
      m_elements.begin(), m_elements.end(), [](WindowElement *e) { return e->isActive(); });
    if (it != m_elements.end()) {
        (*it)->requestActiveChanged();
        Q_EMIT activeChanged();
        return;
    }
    m_elements[0]->requestActiveChanged();
    Q_EMIT activeChanged();
}
