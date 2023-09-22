#pragma once

#include <string>
#include <vector>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>

typedef xcb_window_t XWindow;

struct WmClass
{
    std::string className;
    std::string instanceName;
};

struct WMIcon
{
    uint32_t width;             /** Icon width */
    uint32_t height;            /** Icon height */
    std::vector<uint32_t> data; /** Rows, left to right and top to bottom of the CARDINAL ARGB */
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
    WMIcon getWmIcon(XWindow xid);
    uint32_t getWmPid(XWindow xid);
    std::vector<xcb_atom_t> getWMWindowType(XWindow xid);
    std::string getAtomName(xcb_atom_t atom);

private:
    xcb_connection_t *m_connection;
    xcb_ewmh_connection_t m_ewmh;
};
