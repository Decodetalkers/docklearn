#pragma once

#include "windowelement.h"

#include <QAbstractListModel>
#include <QObject>
#include <QVector>

class ElementGroupModel final : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ElementGroupModel(QObject *parent = nullptr);
    enum ModelRole
    {
        Display = Qt::DisplayRole,
        Icon,
    };
    Q_ENUM(ModelRole)

    void insert(WindowElementGroup *group);

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<WindowElementGroup *> m_groups;
};
