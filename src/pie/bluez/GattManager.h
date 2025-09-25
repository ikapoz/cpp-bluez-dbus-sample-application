/**
* @file GattManager.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/dbus/DBus.h"
#include <pie/logging/Logger.h>

#include <memory>

#include "pie/logging/console_helpers.h"

namespace pie::bluez {
    struct GattManagerData;

    class GattManager {
    public:
        GattManager(
            const std::string &path,
            const std::shared_ptr<pie::dbus::DBus> &dbus,
            const std::shared_ptr<pie::Logger> &logger);

        ~GattManager();

        void register_application(std::string const &path) const;

        void unregister_application(std::string const &path) const;

    private:
        std::shared_ptr<GattManagerData> data;
    };
} // pie

