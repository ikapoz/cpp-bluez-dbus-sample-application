/**
* @file SendDBusMessageExecute.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "SendDBusMessageExecute.h"

namespace pie::dbus {
    SendDBusMessageExecute::SendDBusMessageExecute(std::shared_ptr<DBusMessage> &&msg)
        : DBusMessageExecuteBase(std::move(msg)) {
    }

    void SendDBusMessageExecute::exec(DBusConnection *conn) {
        status(Status::Running);
        DBusError error{};

        uint32_t id{0};
        auto success = dbus_connection_send(conn, msg.get(), &id);
        dbus_connection_flush(conn);
        //dbus_connection_read_write_dispatch(conn, 1);


        auto dbus_result = pie::dbus::DBusResult{};
        if (!success) {
            dbus_result.code = DBusResultCode::Error;
            dbus_result.error = "Failed to send message";
        }
        result(dbus_result, nullptr);
        status(Status::Finished);
    }
} // pie
