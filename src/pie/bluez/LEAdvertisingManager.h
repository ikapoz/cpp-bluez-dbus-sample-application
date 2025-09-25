/**
* @file LEAdvertisingManager.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/dbus/DBusOnMessage.h"
#include "pie/bluez/LEAdvertisement.h"
#include "pie/dbus/DBus.h"

#include <pie/logging/Logger.h>

#include <memory>
#include <string>

namespace pie::bluez {
    struct LEAdvertisingManagerData;

    class LEAdvertisingManager : public dbus::DBusOnMessage {
    public:
        explicit LEAdvertisingManager(
            std::string path,
            std::shared_ptr<dbus::DBus> dbus,
            std::shared_ptr<Logger> logger
        );

        ~LEAdvertisingManager() override;

        bool register_advertisement(const std::shared_ptr<LEAdvertisement> &advertisement);

        void unregister_advertisement(const std::shared_ptr<LEAdvertisement> &advertisement);

        DBusHandlerResult on_message(
            const dbus::DBusMessageInfo &msg_info,
            std::shared_ptr<DBusMessage> message) override;

    private:
        std::shared_ptr<LEAdvertisingManagerData> data;
    };
} // pie::bluez
