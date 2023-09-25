#include "XorgBackground.h"
#include "xcbutils.h"

#include <QBuffer>
#include <QDebug>
#include <QImage>
#include <QRandomGenerator>

#include <cstdio>
#include <cstdlib>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xcb/xproto.h>

#include <string>
#include <string_view>

using namespace std::string_view_literals;

constexpr auto SVG_TEST =
  R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 96 105"> <g fill="#97C024" stroke="#97C024" stroke-linejoin="round" stroke-linecap="round"> <path d="M14,40v24M81,40v24M38,68v24M57,68v24M28,42v31h39v-31z" stroke-width="12"/> <path d="M32,5l5,10M64,5l-6,10 " stroke-width="2"/> </g> <path d="M22,35h51v10h-51zM22,33c0-31,51-31,51,0" fill="#97C024"/> <g fill="#FFF"> <circle cx="36" cy="22" r="2"/> <circle cx="59" cy="22" r="2"/> </g> </svg>)"sv;

XorgBackground::XorgBackground(QObject *parent)
  : QThread(parent)
  , m_run(true)
  , m_xids({})
{
    m_rootWindow = XCBUtils::instance()->getRootWindow();
}

const std::vector<std::string> IGNORED_ATOM = {"_NET_WM_WINDOW_TYPE_NOTIFICATION",
                                               "_NET_WM_WINDOW_TYPE_TOOLTIP",
                                               "_NET_WM_WINDOW_TYPE_POPUP_MENU",
                                               "_NET_WM_WINDOW_TYPE_TOOLBAR"};

void
XorgBackground::stoploop()
{
    m_run = false;
}

void
XorgBackground::run()
{
    handleClientListChanged();

    Display *dyp;
    int screen;
    char *displayName = nullptr;
    Window w;
    XSetWindowAttributes attr;
    XWindowAttributes wattr;

    dyp = XOpenDisplay(displayName);

    if (!dyp) {
        exit(0);
    }

    screen = DefaultScreen(dyp);
    w      = RootWindow(dyp, screen);

    const struct
    {
        const char *name;
        long mask;
    } events[] = {{"keyboard", KeyPressMask | KeyReleaseMask | KeymapStateMask},
                  {"mouse",
                   ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
                     PointerMotionMask | Button1MotionMask | Button2MotionMask | Button3MotionMask |
                     Button4MotionMask | Button5MotionMask | ButtonMotionMask},
                  {"button", ButtonPressMask | ButtonReleaseMask},
                  {"expose", ExposureMask},
                  {"visibility", VisibilityChangeMask},
                  {"structure", StructureNotifyMask},
                  {"substructure", SubstructureNotifyMask | SubstructureRedirectMask},
                  {"focus", FocusChangeMask},
                  {"property", PropertyChangeMask},
                  {"colormap", ColormapChangeMask},
                  {"owner_grab_button", OwnerGrabButtonMask},
                  {nullptr, 0}};

    long mask = 0;
    for (int i = 0; events[i].name; ++i) {
        mask |= events[i].mask;
    }

    attr.event_mask = mask;

    XGetWindowAttributes(dyp, w, &wattr);

    attr.event_mask &= ~SubstructureRedirectMask;

    XSelectInput(dyp, w, attr.event_mask);
    while (m_run) {
        XEvent event;
        XNextEvent(dyp, &event);

        switch (event.type) {
        case DestroyNotify: {
            // XDestroyWindowEvent *eD = (XDestroyWindowEvent *)(&event);
            // Q_EMIT wmDestroyed(XWindow(eD->window));
            break;
        }
        case MapNotify: {
            // XMapEvent *em = (XMapEvent *)(&event);
            // handleNewWindow(XWindow(em->window));
            //  XMapEvent *eM = (XMapEvent *)(&event);

            break;
        }
        case ConfigureNotify: {
            XMapEvent *em = (XMapEvent *)(&event);
            // qDebug() << "Configure is :" << em->window;
            //  std::cout << "ccc" << std::endl;
            //   XConfigureEvent *eC = (XConfigureEvent *)(&event);

            break;
        }
        case PropertyNotify: {
            XPropertyEvent *eP = (XPropertyEvent *)(&event);
            handlePropertyChanged(eP->window, eP->atom);

            // std::cout << "dddd" << std::endl;
            break;
        }
        case UnmapNotify: {
            break;
        }
        default:
            break;
        }
    }

    XCloseDisplay(dyp);
}

void
XorgBackground::handlerootWindowPropertyNotifyEvent(XCBAtom atom)
{
    // qDebug() << QString::fromStdString(XCBUtils::instance()->getAtomName(atom));
    if (atom == XCBUtils::instance()->getAtom("_NET_CLIENT_LIST").value()) {
        handleClientListChanged();
    }
}

void
XorgBackground::handlePropertyChanged(XWindow xid, XCBAtom atom)
{
    if (xid == m_rootWindow) {
        handlerootWindowPropertyNotifyEvent(atom);
    }
}

void
XorgBackground::handleNewWindow(XWindow xid)
{
    if (m_xids.contains(xid)) {
        qDebug() << xid;
        return;
    }
    if (!XCBUtils::instance()->isGoodWindow(xid)) {
        // XCBUtils::instance()->getWmClass(xid).instanceName.length() == 0) {
        return;
    }

    auto atoms = XCBUtils::instance()->getWMWindowType(xid);
    for (std::string atom : IGNORED_ATOM) {
        for (auto mime_atom : atoms) {
            std::string xcbatom = XCBUtils::instance()->getAtomName(mime_atom);
            if (xcbatom == atom) {
                qDebug() << "Not allowed Window";
                return;
            }
        }
    }

    qDebug() << "pid is" << XCBUtils::instance()->getWmPid(xid);
    if (XCBUtils::instance()->getWmClass(xid).instanceName == "dde-dock") {
        return;
    }
    qDebug() << QString::fromStdString(XCBUtils::instance()->getWmClass(xid).instanceName);
    m_xids.insert(xid);

    qDebug() << XCBUtils::instance()->getWmIcon(xid).data.size();
    quint64 id            = QRandomGenerator::global()->generate64();
    WindowElement *window = new WindowElement(QString::number(id));
    window->setIcon(generateIconData(xid));
    Q_EMIT windowGenerated(window);
    connect(this, &XorgBackground::wmDestroyed, window, [window, xid, this](XWindow newid) {
        if (newid != xid) {
            return;
        }
        m_xids.remove(xid);
        window->deleteSelf();
    });
}

void
XorgBackground::handleClientListChanged()
{
    QSet<XWindow> newClientList, oldClientList, addClientList, rmClientList;
    for (auto atom : XCBUtils::instance()->getClientList()) {
        newClientList.insert(atom);
    }
    for (auto atom : m_xids) {
        oldClientList.insert(atom);
    }

    addClientList = newClientList - oldClientList;
    rmClientList  = oldClientList - newClientList;
    if (!addClientList.isEmpty()) {
        for (auto xid : addClientList) {
            handleNewWindow(xid);
        }
    }

    for (auto xid : rmClientList) {
        Q_EMIT wmDestroyed(xid);
    }
}

QString
XorgBackground::generateIconData(XWindow xid)
{
    WMIcon icon = XCBUtils::instance()->getWmIcon(xid);
    if (icon.width == 0) {
        return QByteArray::fromStdString(SVG_TEST.data()).toBase64();
    }
    QImage image = QImage((uchar *)icon.data.data(), icon.width, icon.width, QImage::Format_ARGB32);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    image.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    image.save(&buffer, "PNG");

    QString encode = buffer.data().toBase64();
    buffer.close();
    return encode;
}
