#pragma once

#include "models/windowelement.h"

#include <QThread>
#include <QVector>

#include <xcb/xproto.h>

typedef xcb_window_t XWindow;

class XorgBackground final : public QThread
{
    Q_OBJECT
public:
    explicit XorgBackground(QObject *parent);

    void stoploop();

protected:
    void run() override;

private:
    void handleMapNotifyEvent(XWindow xid);

signals:
    void windowGenerated(WindowElement *window);

private:
    bool m_run;
    QVector<XWindow> xids;
};
