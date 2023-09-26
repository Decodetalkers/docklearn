#pragma once

#include "models/windowelement.h"

#include <QThread>
#include <QVector>
#include <QSet>

#include <xcb/xproto.h>

typedef xcb_atom_t XCBAtom;
typedef xcb_window_t XWindow;

class XorgBackground final : public QThread
{
    Q_OBJECT
public:
    static XorgBackground *instance()
    {
        static XorgBackground instance;
        return &instance;
    }

    explicit XorgBackground(QObject *parent = nullptr);

    void stoploop();

protected:
    void run() override;

private:
    void handleNewWindow(XWindow xid);
    void handlePropertyChanged(XWindow xid, XCBAtom atom);

    void handlerootWindowPropertyNotifyEvent(XCBAtom atom);
    void handleClientListChanged();
    QString generateIconData(XWindow xid);

signals:
    void windowGenerated(WindowElement *window);
    void wmDestroyed(XWindow xid);

private:
    XWindow m_rootWindow;
    bool m_run;
    QSet<XWindow> m_xids;
};
