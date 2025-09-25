/**
* @file le_advertising_manager.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "le_advertising_manager.h"
#include "pie/bluez/helper/bluez.h"

namespace {
    std::shared_ptr<pie::bluez::LEAdvertisingManager> le_advertising_manager_hci0{nullptr};
}

namespace pie::bluez::le_advertising_manager {
    std::shared_ptr<pie::bluez::LEAdvertisingManager> hci0(
        const std::shared_ptr<pie::dbus::DBus> &dbus,
        const std::shared_ptr<pie::Logger> &logger) {
        if (le_advertising_manager_hci0 == nullptr)
            le_advertising_manager_hci0 = std::make_shared<pie::bluez::LEAdvertisingManager>(
                pie::bluez::path_org_bluez_hci0, dbus, logger);

        return le_advertising_manager_hci0;
    }

    bool is_interface(const pie::dbus::DBusMessageInfo &msg_info) {
        return msg_info.iface == iface;
    }
}
