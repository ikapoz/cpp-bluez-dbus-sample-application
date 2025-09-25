/**
* @file DBusMessageExecuteBase.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "DBusMessageExecuteBase.h"

#include <mutex>
#include <thread>

namespace {
    std::chrono::milliseconds wait_time = 1ms;


    std::tuple<bool, std::unique_lock<std::shared_mutex> > lock(std::shared_mutex &mutex) {
        auto cnt{0};
        auto max_cnt{10};
        while (cnt < max_cnt) {
            try {
                std::unique_lock<std::shared_mutex> lock(mutex);
                return {true, std::move(lock)};
            } catch (...) {
            }
            std::this_thread::sleep_for(wait_time);
            ++cnt;
        }

        return {false, std::unique_lock<std::shared_mutex>()};
    }
}


namespace pie::dbus {
    DBusMessageExecuteBase::DBusMessageExecuteBase(std::shared_ptr<DBusMessage> &&msg,
                                                   std::chrono::milliseconds max_wait_time) : msg(msg),
        max_wait_time(max_wait_time) {
    };

    DBusMessageExecuteBase::Status DBusMessageExecuteBase::status() {
        auto [success, locker] = lock(mutex);
        if (success)
            return status_;

        return DBusMessageExecuteBase::Status::New;;
    }

    std::optional<std::tuple<DBusResult, std::shared_ptr<DBusMessage> > > DBusMessageExecuteBase::result() {
        auto [success, locker] = lock(mutex);
        if (success)
            return result_;

        return std::nullopt;
    }

    void DBusMessageExecuteBase::status(Status status) {
        auto [success, locker] = lock(mutex);
        if (success)
            status_ = status;
    }


    void DBusMessageExecuteBase::result(DBusResult result, std::shared_ptr<DBusMessage> msg) {
        auto [success, locker] = lock(mutex);
        if (success)
            result_ = {result, msg};
    }
} // pie
