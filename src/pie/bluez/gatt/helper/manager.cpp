/**
* @file manager.cpp
 * @author Ilija Poznic
 * @date 2025
 */

#include "manager.h"

namespace pie::bluez::gatt::manager {
    std::string to_string(Methods method) {
        switch (method) {
            case Methods::RegisterApplication:
                return "RegisterApplication";
            case Methods::UnregisterApplication:
                return "UnregisterApplication";
            default:
                return "Unknown";
        }
    }

    bool is_method(
        const dbus::DBusMessageInfo &msg_info,
        const std::string &path,
        Methods method) {
        if (msg_info.iface == iface &&
            msg_info.path == path &&
            msg_info.member == to_string(method))
            return true;

        return false;
    }
} // pie
