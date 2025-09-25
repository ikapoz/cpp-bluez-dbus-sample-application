/**
* @file DBusMessageExecuteBase.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "pie/dbus/DBus.h"

#include <dbus/dbus.h>

#include <memory>
#include <tuple>
#include <optional>
#include <shared_mutex>
#include <chrono>

//std::chrono::milliseconds max_wait_time = 25ms;

namespace pie::dbus {
    class DBusMessageExecuteBase {
    public:
        enum class Status {
            New,
            Running,
            Finished
        };

        explicit DBusMessageExecuteBase(
            std::shared_ptr<DBusMessage> &&msg, std::chrono::milliseconds max_wait_time = 25ms);

        virtual ~DBusMessageExecuteBase() = default;

        virtual void exec(DBusConnection *conn) = 0;

        Status status();

        std::optional<std::tuple<DBusResult, std::shared_ptr<DBusMessage> > > result();

    protected:
        void status(Status status);

        void result(DBusResult result, std::shared_ptr<DBusMessage> msg);

        Status status_{Status::New};

        std::optional<std::tuple<DBusResult, std::shared_ptr<DBusMessage> > > result_;

        std::shared_mutex mutex{};

        std::shared_ptr<DBusMessage> msg;

        std::chrono::milliseconds max_wait_time;
    };
} // pie
