/**
* @file Characteristic.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/dbus/DBus.h"
#include "pie/dbus/DBusObjectManager.h"
#include "pie/dbus/DBusOnMessage.h"
#include "OnValueChanged.h"

#include <pie/logging/Logger.h>

#include "helper/characteristic.h"

namespace pie::bluez::gatt {
    struct CharacteristicData;

    class Service;

    class Characteristic : public dbus::DBusObjectManager, dbus::DBusOnMessage {
    public:
        explicit Characteristic(const std::string &uuid,
                                const std::weak_ptr<Service> &service,
                                std::vector<pie::bluez::gatt::characteristic::Flag> &&flags,
                                const std::shared_ptr<OnValueChanged> &subscriber,
                                const std::shared_ptr<pie::dbus::DBus> &dbus,
                                const std::shared_ptr<pie::Logger> &logger);

        ~Characteristic() override;

        [[nodiscard]] const std::string &path() const;

        [[nodiscard]] const std::string &uuid() const;

        void get_managed_objects(DBusMessageIter *iter) override;

        DBusHandlerResult on_message(
            const dbus::DBusMessageInfo &msg_info, std::shared_ptr<DBusMessage> message) override;

    private:
        std::shared_ptr<CharacteristicData> data;
    };
} // pie
