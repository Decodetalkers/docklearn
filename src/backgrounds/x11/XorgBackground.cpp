#include "XorgBackground.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xcb/xproto.h>

#include <QRandomGenerator>

#include <cstdio>
#include <cstdlib>

#include <string>
#include <string_view>

using namespace std::string_view_literals;

constexpr auto SVG_TEST =
  R"("data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 96 105\"> <g fill=\"#97C024\" stroke=\"#97C024\" stroke-linejoin=\"round\" stroke-linecap=\"round\"> <path d=\"M14,40v24M81,40v24M38,68v24M57,68v24M28,42v31h39v-31z\" stroke-width=\"12\"/> <path d=\"M32,5l5,10M64,5l-6,10 \" stroke-width=\"2\"/> </g> <path d=\"M22,35h51v10h-51zM22,33c0-31,51-31,51,0\" fill=\"#97C024\"/> <g fill=\"#FFF\"> <circle cx=\"36\" cy=\"22\" r=\"2\"/> <circle cx=\"59\" cy=\"22\" r=\"2\"/> </g> </svg>")"sv;
typedef xcb_window_t XWindow;

XorgBackground::XorgBackground(QObject *parent)
  : QThread(parent)
{
}

void
XorgBackground::run()
{
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
    while (true) {
        XEvent event;
        XNextEvent(dyp, &event);

        switch (event.type) {
        case DestroyNotify: {
            // XDestroyWindowEvent *eD = (XDestroyWindowEvent *)(&event);
            break;
        }
        case MapNotify: {
            quint64 id            = QRandomGenerator::global()->generate64();
            WindowElement *window = new WindowElement(QString::number(id));
            window->setIcon(QString::fromStdString(SVG_TEST.data()));
            Q_EMIT windowGenerated(window);
            // XMapEvent *eM = (XMapEvent *)(&event);

            break;
        }
        case ConfigureNotify: {
            // std::cout << "ccc" << std::endl;
            //  XConfigureEvent *eC = (XConfigureEvent *)(&event);

            break;
        }
        case PropertyNotify: {
            // XPropertyEvent *eP = (XPropertyEvent *)(&event);

            // std::cout << "dddd" << std::endl;
            break;
        }
        case UnmapNotify: {
            // std::cout << "eeee" << std::endl;
            break;
        }
        default:
            break;
        }
    }

    XCloseDisplay(dyp);
}