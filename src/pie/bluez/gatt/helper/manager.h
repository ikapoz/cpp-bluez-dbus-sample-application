/**
 * @file manager.h
 * @author Ilija Poznic
 * @date 2025
 */

#pragma once

#include "pie/dbus/DBus.h"

namespace pie::bluez::gatt::manager {
    inline const char *iface = "org.bluez.GattManager1";

    enum class Methods {
        RegisterApplication,
        UnregisterApplication,
        Unknown
    };

    std::string to_string(Methods method);

    bool is_method(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Methods method);
}
