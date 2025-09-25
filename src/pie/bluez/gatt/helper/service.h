/**
* @file service.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/dbus/DBus.h"

#include <string>


namespace pie::bluez::gatt::service {
    inline const char *iface = "org.bluez.GattService1";

    bool is_interface(const pie::dbus::DBusMessageInfo &msg_info);

    enum class Property {
        UUID,
        Primary,
        Includes,
        Characteristics,
        Unknown
    };

    Property to_property(const char *property_name);

    std::string to_string(Property property);
}
