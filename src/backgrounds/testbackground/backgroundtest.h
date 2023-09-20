#pragma once

#include <QObject>

class TestBackground final : public QObject
{
    Q_OBJECT

public:
    explicit TestBackground(QObject *parent = nullptr);
    ~TestBackground() = default;
};
