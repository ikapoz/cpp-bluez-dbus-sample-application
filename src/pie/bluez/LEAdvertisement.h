/**
* @file LEAdvertisement.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include <vector>

#include "pie/dbus/DBus.h"
#include "pie/dbus/DBusOnMessage.h"

#include "pie/logging/Logger.h"

namespace pie::bluez {
    struct LEAdvertisementData;

    enum class LEAdvertisementType {
        Broadcast,
        Peripheral,
        Unknown
    };

    class LEAdvertisement : public dbus::DBusOnMessage {
    public:
        explicit LEAdvertisement(
            std::string path,
            std::shared_ptr<pie::dbus::DBus> dbus,
            std::shared_ptr<pie::Logger> logger);

        ~LEAdvertisement() override;

        void register_advertisement(std::shared_ptr<DBusMessage> msg);

        void unregister_advertisement(std::shared_ptr<DBusMessage> msg);

        void type(LEAdvertisementType set);

        LEAdvertisementType type();

        void service_uuids(std::vector<std::string> set);

        std::vector<std::string> service_uuids();

        std::string name();

        void name(std::string set);


        DBusHandlerResult
        on_message(
            const dbus::DBusMessageInfo &msg_info, std::shared_ptr<DBusMessage> message) override;

    private:
        std::shared_ptr<LEAdvertisementData> data;
    };
} // pie::bluez
