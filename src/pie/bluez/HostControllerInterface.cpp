/**
* @file HostControllerInterface.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "HostControllerInterface.h"

#include <pie/logging/console_helpers.h>

namespace pie::bluez {
    struct HostControllerInterfaceData {
        std::string path;
        std::shared_ptr<pie::dbus::DBus> dbus;
        std::shared_ptr<Logger> logger;

        // ifaces
        std::shared_ptr<pie::bluez::GattManager> gatt_manager;
        std::shared_ptr<pie::bluez::LEAdvertisingManager> le_advertising_manager;
    };
} // pie


namespace {
    inline const std::string TAG = "HostControllerInterface";
}


namespace pie::bluez {
    HostControllerInterface::HostControllerInterface(const std::string& path, const std::shared_ptr<pie::dbus::DBus>& dbus,
                                                     const std::shared_ptr<pie::Logger>& logger) {
        data = std::make_shared<HostControllerInterfaceData>();
        data->path = path;
        data->dbus = dbus;
        data->logger = logger;

        data->gatt_manager = std::make_shared<GattManager>(path, dbus, logger);
        data->le_advertising_manager = std::make_shared<LEAdvertisingManager>(path, dbus, logger);
    }

    HostControllerInterface::~HostControllerInterface() {
        pie::logger::log_if_debug(data->logger, LogLevel::Trace, "HostControllerInterface::~HostControllerInterface()");
        data = nullptr;
    }


    const std::shared_ptr<pie::bluez::GattManager> &HostControllerInterface::gatt_manager() {
        return data->gatt_manager;
    }

    const std::shared_ptr<pie::bluez::LEAdvertisingManager> &HostControllerInterface::le_advertising_manager() {
        return data->le_advertising_manager;
    }


} // pie
