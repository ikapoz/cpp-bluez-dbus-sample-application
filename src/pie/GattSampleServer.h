/**
* @file GattSampleServer.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/bluez/gatt/Server.h"
#include "pie/dbus/DBusOnMessage.h"
#include "pie/bluez/gatt/OnValueChanged.h"
#include "pie/dbus/DBus.h"

#include <pie/logging/Logger.h>

#include <memory>

namespace pie {
    inline const std::string service_uuid("23500001-da00-49ad-9923-296889f1d83d");
    inline const std::string rx_uuid("23500002-da00-49ad-9923-296889f1d83d");

    struct GattSampleServerData;

    class GattSampleServer : public bluez::gatt::Server, public dbus::DBusOnMessage,
                             public bluez::gatt::OnValueChanged {
    public:
        explicit GattSampleServer(
            std::shared_ptr<pie::dbus::DBus> const &dbus,
            std::shared_ptr<Logger> const &logger);

        ~GattSampleServer() override;

        void start() override;

        void stop() override;

        [[nodiscard]] bluez::gatt::ServerState state() const override;

        void on_value_changed(const std::string &uuid, const std::vector<uint8_t> &value) override;

        DBusHandlerResult on_message(
            const dbus::DBusMessageInfo &msg_info,
            std::shared_ptr<DBusMessage> message) override;

    private:
        std::shared_ptr<GattSampleServerData> data;
    };
} // pie
