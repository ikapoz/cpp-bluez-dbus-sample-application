/**
* @file le_advertising_manager.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/dbus/DBus.h"
#include "pie/bluez/LEAdvertisingManager.h"

namespace pie::bluez::le_advertising_manager {
    inline const char *iface = "org.bluez.LEAdvertisingManager1";

    std::shared_ptr<pie::bluez::LEAdvertisingManager> hci0(
        const std::shared_ptr<pie::dbus::DBus> &dbus,
        const std::shared_ptr<pie::Logger> &logger);

    bool is_interface(const pie::dbus::DBusMessageInfo &msg_info);
}
