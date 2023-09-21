#pragma once

#include <string>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>

typedef xcb_window_t XWindow;

struct WmClass {
    std::string className;
    std::string instanceName;
};

class XCBUtils
{
public:
    XCBUtils();
    XCBUtils(XCBUtils &&)                 = default;
    XCBUtils(const XCBUtils &)            = default;
    XCBUtils &operator=(XCBUtils &&)      = default;
    XCBUtils &operator=(const XCBUtils &) = default;
    ~XCBUtils();

public:
    static XCBUtils *instance()
    {
        static XCBUtils instance;
        return &instance;
    }

    WmClass getWmClass(XWindow xid);
    bool isGoodWindow(XWindow xid);

private:
    xcb_connection_t *m_connection;
};
