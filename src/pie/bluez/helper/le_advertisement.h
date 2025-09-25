/**
* @file le_advertisement.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/bluez/LEAdvertisement.h"
#include "pie/dbus/DBus.h"

#include <string>
#include <memory>

namespace pie::bluez::le_advertisement {
    inline const char *iface = "org.bluez.LEAdvertisement1";

    // std::shared_ptr<pie::bluez::LEAdvertisement> hci0(const std::shared_ptr<pie::Logger> &logger);

    bool is_interface(const pie::dbus::DBusMessageInfo &msg_info);

    std::string to_string(pie::bluez::LEAdvertisementType type);

    enum class Property {
        Type,
        ServiceUUIDs,
        ManufacturerData,
        SolicitUUIDs,
        ServiceData,
        Data,
        Includes,
        LocalName,
        Duration,
        Timeout,
        Unknown
    };

    Property to_property(const char *property_name);

    std::string to_string(Property property);
} // pie
