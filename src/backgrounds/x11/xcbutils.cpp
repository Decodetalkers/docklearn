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

WmClass
XCBUtils::getWmClass(XWindow xid)
{
    WmClass ret;
    xcb_get_property_cookie_t cookie = xcb_icccm_get_wm_class(m_connection, xid);
    xcb_icccm_get_wm_class_reply_t reply;
    reply.instance_name = nullptr;
    reply.class_name    = nullptr;
    xcb_icccm_get_wm_class_reply(m_connection,
                                 cookie,
                                 &reply,
                                 nullptr); // 返回值为0不一定表示失败， 故不做返回值判断
    if (reply.class_name)
        ret.className.assign(reply.class_name);

    if (reply.instance_name)
        ret.instanceName.assign(reply.instance_name);

    if (reply.class_name || reply.instance_name) {
        xcb_icccm_get_wm_class_reply_wipe(&reply);
    }

    return ret;
}

bool
XCBUtils::isGoodWindow(XWindow xid)
{
    bool ret                         = false;
    xcb_get_geometry_cookie_t cookie = xcb_get_geometry(m_connection, xid);
    xcb_generic_error_t **errStore   = nullptr;
    xcb_get_geometry_reply_t *reply  = xcb_get_geometry_reply(m_connection, cookie, errStore);
    if (reply) {
        // 正常获取窗口geometry则判定为good
        if (!errStore) {
            ret = true;
        } else {
            free(errStore);
        }

        free(reply);
    }
    return ret;
}
