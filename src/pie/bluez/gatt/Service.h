/**
* @file Service.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/dbus/DBus.h"
#include "pie/dbus/DBusObjectManager.h"

#include <pie/logging/Logger.h>

#include <memory>

namespace pie::bluez::gatt {
    struct ServiceData;

    class Characteristic;

    class Service : public dbus::DBusObjectManager {
    public:
        explicit Service(std::string uuid,
                         const std::string& base_path,
                         bool is_primary,
                         const std::shared_ptr<pie::dbus::DBus> &dbus,
                         const std::shared_ptr<pie::Logger> &logger);

        ~Service() override;

        [[nodiscard]] const std::string &path() const;

        void add_characteristic(std::weak_ptr<Characteristic> characteristic);

        void get_managed_objects(DBusMessageIter *iter) override;

    private:
        std::shared_ptr<ServiceData> data;
    };
} // pie
