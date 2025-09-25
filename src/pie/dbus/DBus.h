/**
* @file DBus.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/dbus/DBusOnMessage.h"
#include "pie/dbus/helper/dbus.h"

#include <pie/logging/Logger.h>

#include <chrono>
#include <string>
#include <memory>

using namespace std::chrono_literals;

namespace pie::dbus {
    enum class DBusState {
        Error,
        Initializing,
        Running,
        Stopped
    };

    enum class DBusResultCode {
        Success,
        Error,
        E_NotOnDBusThread,
        E_CMD_Timeout
    };

    struct DBusResult {
        DBusResultCode code{DBusResultCode::Success};
        std::string error;
    };

    struct DBusMessageResult : DBusResult {
        // DBusResultCode code{DBusResultCode::Success};
        // std::string error;
        std::shared_ptr<DBusMessage> message{nullptr};
    };

    struct DBusData;

    class DBus {
    public:
        explicit DBus(const std::shared_ptr<pie::Logger> &logger);

        ~DBus();

        DBus(const DBus &) = delete;

        DBus &operator=(const DBus &) = delete;

        DBus(DBus &&) = delete;

        DBus &operator=(DBus &&) = delete;

        [[nodiscard]] DBusState state() const;

        void subscribe(const std::weak_ptr<pie::dbus::DBusOnMessage> &subscriber);

        std::tuple<DBusResult, std::shared_ptr<DBusMessage> > send_with_reply(
            std::shared_ptr<DBusMessage> &&msg,
            std::chrono::milliseconds max_wait_time = 25ms);

        DBusResult send(std::shared_ptr<DBusMessage> &&msg,
                        std::chrono::milliseconds max_wait_time = 25ms);

        DBusResult reply(std::shared_ptr<DBusMessage> &&msg);

        static std::shared_ptr<DBusMessage> new_message(std::string &bus_name, std::string &path, std::string &iface,
                                                        std::string &method);

    private:
        std::shared_ptr<DBusData> data;

        void execute();
    };
} // pie
