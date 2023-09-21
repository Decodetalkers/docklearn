#pragma once


#include "windowelement.h"

#include "backgrounds/x11/XorgBackground.h"

#include <QObject>
#include <QVector>

class ElementGroupModel final : public QObject
{
    Q_OBJECT

public:
    explicit ElementGroupModel(QObject *parent = nullptr);
    ~ElementGroupModel();

    Q_PROPERTY(QVector<WindowElementGroup *> groups READ groups NOTIFY groupsChanged)
    inline QVector<WindowElementGroup *> groups() { return m_groups; }

public slots:
    void insert(WindowElement *group);
    void remove(WindowElementGroup *group);

signals:
    void groupsChanged();

private:
    XorgBackground *m_xorgBackground;
    QVector<WindowElementGroup *> m_groups;
};
