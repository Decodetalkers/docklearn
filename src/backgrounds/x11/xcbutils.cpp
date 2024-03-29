#include "xcbutils.h"

#include <QDebug>
#include <memory>

#include <X11/Xlib.h>
#include <X11/extensions/XRes.h>

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
XWindow
XCBUtils::getRootWindow()
{
    XWindow rootWindow = 0;
    /* Get the first screen */
    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(m_connection)).data;
    if (screen) {
        rootWindow = screen->root;
    }

    return rootWindow;
}

std::list<XWindow>
XCBUtils::getClientList()
{
    std::list<XWindow> ret;
    xcb_get_property_cookie_t cookie = xcb_ewmh_get_client_list_stacking(&m_ewmh, 0);
    xcb_ewmh_get_windows_reply_t reply;
    if (xcb_ewmh_get_client_list_stacking_reply(&m_ewmh, cookie, &reply, nullptr)) {
        for (uint8_t i = 0; i < reply.windows_len; ++i) {
            ret.push_back(reply.windows[i]);
        }

        xcb_ewmh_get_windows_reply_wipe(&reply);
    }

    return ret;
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

uint32_t
XCBUtils::getWmPid(XWindow xid)
{
    // NOTE(black_desk): code copy from
    // https://gitlab.gnome.org/GNOME/metacity/-/merge_requests/13/diffs

    XResClientIdSpec spec = {
      .client = xid,
      .mask   = XRES_CLIENT_ID_PID_MASK,
    };

    std::shared_ptr<Display> dpy = {
      XOpenDisplay(nullptr),
      [](Display *p) { XCloseDisplay(p); },
    };

    long num_ids;
    XResClientIdValue *client_ids;
    XResQueryClientIds(dpy.get(), 1, &spec, &num_ids, &client_ids);

    pid_t pid = -1;
    for (long i = 0; i < num_ids; i++) {
        qDebug() << XResGetClientPid(&client_ids[i]);
        if (client_ids[i].spec.mask == XRES_CLIENT_ID_PID_MASK) {
            pid = XResGetClientPid(&client_ids[i]);
            break;
        }
    }

    XResClientIdsDestroy(num_ids, client_ids);
    return pid;
}

std::vector<xcb_atom_t>
XCBUtils::getWMWindowType(XWindow xid)
{
    std::vector<xcb_atom_t> ret;
    xcb_get_property_cookie_t cookie = xcb_ewmh_get_wm_window_type(&m_ewmh, xid);
    xcb_ewmh_get_atoms_reply_t reply;
    if (xcb_ewmh_get_wm_window_type_reply(&m_ewmh, cookie, &reply, nullptr)) {
        for (uint32_t i = 0; i < reply.atoms_len; i++) {
            ret.push_back(reply.atoms[i]);
        }
        xcb_ewmh_get_atoms_reply_wipe(&reply);
    }
    return ret;
}

std::string
XCBUtils::getAtomName(xcb_atom_t atom)
{
    std::string ret;
    xcb_get_atom_name_cookie_t cookie = xcb_get_atom_name(m_connection, atom);
    std::shared_ptr<xcb_get_atom_name_reply_t> reply(
      xcb_get_atom_name_reply(m_connection, cookie, nullptr),
      [=](xcb_get_atom_name_reply_t *reply) { free(reply); });
    if (reply) {
        char *name = xcb_get_atom_name_name(reply.get());
        ret        = name;
    }

    return ret;
}

std::optional<xcb_atom_t>
XCBUtils::getAtom(const char *name)
{
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(m_connection, false, strlen(name), name);
    std::shared_ptr<xcb_intern_atom_reply_t> reply(
      xcb_intern_atom_reply(m_connection, cookie, nullptr),
      [=](xcb_intern_atom_reply_t *reply) { free(reply); });
    if (reply) {
        return reply->atom;
    }
    return std::nullopt;
}
