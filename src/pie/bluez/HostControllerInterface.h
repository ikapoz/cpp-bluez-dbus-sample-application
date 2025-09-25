/**
* @file HostControllerInterface.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/dbus/DBus.h"
#include "pie/logging/Logger.h"
#include "GattManager.h"
#include "LEAdvertisingManager.h"

#include <memory>

namespace pie::bluez {
    struct HostControllerInterfaceData;

    class HostControllerInterface {
    public:
        HostControllerInterface(
            const std::string& path,
            const std::shared_ptr<pie::dbus::DBus>& dbus,
            const std::shared_ptr<pie::Logger>& logger
        );

        ~HostControllerInterface();

        const std::shared_ptr<pie::bluez::GattManager> &gatt_manager();

        const std::shared_ptr<pie::bluez::LEAdvertisingManager> &le_advertising_manager();

    private:
        std::shared_ptr<HostControllerInterfaceData> data;
    };
} // pie
