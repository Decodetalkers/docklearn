#pragma once

#include "models/windowelement.h"

#include <QThread>
#include <QVector>
#include <QSet>

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
    void handleNewWindow(XWindow xid);
    void handlePropertyChanged(XWindow xid);

    void handleClientListChanged();

signals:
    void windowGenerated(WindowElement *window);
    void wmDestroyed(XWindow xid);

private:
    XWindow m_rootWindow;
    bool m_run;
    QSet<XWindow> m_xids;
};
