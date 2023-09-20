#pragma once


#include "windowelement.h"

#include <QObject>
#include <QVector>

class ElementGroupModel final : public QObject
{
    Q_OBJECT

public:
    explicit ElementGroupModel(QObject *parent = nullptr);

    Q_PROPERTY(QVector<WindowElementGroup *> groups READ groups NOTIFY groupsChanged)
    inline QVector<WindowElementGroup *> groups() { return m_groups; }

public slots:
    void insert(WindowElement *group);
    void remove(WindowElementGroup *group);

signals:
    void groupsChanged();

private:
    QVector<WindowElementGroup *> m_groups;
};
