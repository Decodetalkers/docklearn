#include "xcbutils.h"

XCBUtils::XCBUtils()
{
    m_connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(m_connection)) {
        return;
    }
    // TODO:
}

XCBUtils::~XCBUtils()
{
    if (m_connection) {
        xcb_disconnect(m_connection);
        m_connection = nullptr;
    }
}

std::string
XCBUtils::getWmClass(XWindow xid)
{
    std::string className;
    xcb_get_property_cookie_t cookie = xcb_icccm_get_wm_class(m_connection, xid);
    xcb_icccm_get_wm_class_reply_t reply;
    reply.instance_name = nullptr;
    reply.class_name    = nullptr;
    xcb_icccm_get_wm_class_reply(m_connection, cookie, &reply, nullptr);
    if (reply.class_name) {
        className.assign(reply.class_name);
        xcb_icccm_get_wm_class_reply_wipe(&reply);
    }
    return className;
}
