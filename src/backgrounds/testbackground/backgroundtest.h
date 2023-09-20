#pragma once

#include "models/windowelement.h"

#include <QObject>

class TestBackground final : public QObject
{
    Q_OBJECT

public:
    explicit TestBackground(QObject *parent = nullptr);
    ~TestBackground() = default;

signals:
    void windowGenerated(WindowElement *window);
private:
    int m_count = 0;
};
