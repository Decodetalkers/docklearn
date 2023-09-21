#pragma once

#include "models/windowelement.h"
#include <QThread>

class XorgBackground final : public QThread
{
    Q_OBJECT
public:
    explicit XorgBackground(QObject *parent);

protected:
    void run() override;

signals:
    void windowGenerated(WindowElement *window);
};
