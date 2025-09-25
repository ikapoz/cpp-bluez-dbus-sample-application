/**
* @file DBusOnMessage.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include <dbus/dbus.h>

#include <string>
#include <iostream>
#include <memory>
#include <optional>

namespace pie::dbus {
    enum class DBusMessageType {
        Invalid = 0,
        MethodCall = 1,
        MethodReturn = 2,
        Signal = 4,
        Unknown = 100
    };

    std::ostream &operator<<(std::ostream &os, DBusMessageType type);

    struct DBusMessageInfo {
        DBusMessageType type;
        std::string destination;
        std::string path;
        std::string iface;
        std::string member;
        uint32_t serial{0};
        std::optional<uint32_t> reply_serial{0};
    };

    std::ostream &operator<<(std::ostream &os, DBusMessageInfo &info);

    class DBusOnMessage {
    public:
        virtual ~DBusOnMessage() = default;

        virtual DBusHandlerResult on_message(const DBusMessageInfo &msg_info,
                                             std::shared_ptr<DBusMessage> message) = 0;
    };
}
