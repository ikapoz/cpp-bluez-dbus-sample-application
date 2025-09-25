/**
* @file characteristic.h
 * @author Ilija Poznic
 * @date 2025
 */

#pragma once

#include "pie/dbus/DBus.h"

namespace pie::bluez::gatt::characteristic {
    inline const char *iface = "org.bluez.GattCharacteristic1";

    bool is_interface(const pie::dbus::DBusMessageInfo &msg_info);

    enum class Property {
        UUID,
        Service,
        Flags,
        Descriptors,
        Value,
        Unknown
    };

    std::string to_string(Property property);

    enum class Flag {
        Broadcast,
        Read,
        WriteWithoutResponse,
        Write,
        Notify,
        Indicate,
        Unknown
    };

    std::string to_string(Flag flag);

    enum class Methods {
        ReadValue,
        WriteValue,
        Unknown
    };

    bool is_method(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Methods method);
} // pie
