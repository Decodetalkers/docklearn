#include "xcbutils.h"
#include <QDebug>

XCBUtils::XCBUtils()
{
    m_connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(m_connection)) {
        return;
    }

    if (!xcb_ewmh_init_atoms_replies(
          &m_ewmh, xcb_ewmh_init_atoms(m_connection, &m_ewmh), nullptr)) {
        qDebug() << "Error Connect m_ewmh";
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

WMIcon
XCBUtils::getWmIcon(XWindow xid)
{
    WMIcon wmIcon{};

    xcb_get_property_cookie_t cookie = xcb_ewmh_get_wm_icon(&m_ewmh, xid);
    xcb_ewmh_get_wm_icon_reply_t reply;
    xcb_generic_error_t *error;

    auto ret = xcb_ewmh_get_wm_icon_reply(&m_ewmh, cookie, &reply, &error);

    if (error) {
        qDebug() << "failed to get wm Icon" << error->error_code;
        std::free(error);
        return wmIcon;
    }

    if (!ret) {
        return wmIcon;
    }

    auto fcn = [](xcb_ewmh_wm_icon_iterator_t it) {
        const auto size = it.width * it.height;
        std::vector<uint32_t> ret(size);

        uint32_t *data = it.data;
        if (!data) {
            return ret;
        }

        std::copy_n(data, size, ret.begin());
        return ret;
    };

    xcb_ewmh_wm_icon_iterator_t iter = xcb_ewmh_get_wm_icon_iterator(&reply);
    xcb_ewmh_wm_icon_iterator_t wmIconIt{0, 0, nullptr};
    for (; iter.rem; xcb_ewmh_get_wm_icon_next(&iter)) {
        const uint32_t size = iter.width * iter.height;
        if (size > 0 && size > wmIconIt.width * wmIconIt.height) {
            wmIconIt = iter;
        }
    }

    wmIcon = WMIcon{wmIconIt.width, wmIconIt.height, fcn(wmIconIt)};

    xcb_ewmh_get_wm_icon_reply_wipe(&reply);

    return wmIcon;
}
