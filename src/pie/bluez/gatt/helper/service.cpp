/**
* @file service.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "service.h"

#include <cstring>

namespace pie::bluez::gatt::service {
    bool is_interface(const pie::dbus::DBusMessageInfo &msg_info) {
        return msg_info.iface == iface;
    }

    Property to_property(const char *property_name) {
        if (strcmp(property_name, "UUID") == 0)
            return Property::UUID;

        if (strcmp(property_name, "Primary") == 0)
            return Property::Primary;

        if (strcmp(property_name, "Includes") == 0)
            return Property::Includes;

        if (strcmp(property_name, "Characteristics") == 0)
            return Property::Characteristics;

        return Property::Unknown;
    }

    std::string to_string(Property property) {
        switch (property) {
            case Property::UUID:
                return "UUID";
            case Property::Primary:
                return "Primary";
            case Property::Includes:
                return "Includes";
            case Property::Characteristics:
                return "Characteristics";
            default:
                return "Unknown";
        }
    }
}
