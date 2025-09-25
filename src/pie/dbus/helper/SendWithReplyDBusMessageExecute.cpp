/**
* @file SendWithReplyDBusMessageExecute.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "SendWithReplyDBusMessageExecute.h"

#include "dbus.h"

namespace pie::dbus {
    SendWithReplyDBusMessageExecute::SendWithReplyDBusMessageExecute(
        std::shared_ptr<DBusMessage> &&msg) : DBusMessageExecuteBase(std::move(msg)) {
    }

    void SendWithReplyDBusMessageExecute::exec(DBusConnection *conn) {
        status(Status::Running);
        DBusError error{};
        auto msg_rsp_p = dbus_connection_send_with_reply_and_block(conn, msg.get(), max_wait_time.count(), &error);
        std::shared_ptr<DBusMessage> msg_rsp(msg_rsp_p, [](DBusMessage *msg) {
            if (msg)
                dbus_message_unref(msg);
        });
        auto dbus_result = pie::dbus::parse(&error);
        result(dbus_result, msg_rsp);
        status(Status::Finished);
    }
}
