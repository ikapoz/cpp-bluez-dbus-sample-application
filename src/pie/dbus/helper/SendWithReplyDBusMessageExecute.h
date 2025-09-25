/**
* @file SendWithReplyDBusMessageExecute.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "DBusMessageExecuteBase.h"

namespace pie::dbus {
    class SendWithReplyDBusMessageExecute : public DBusMessageExecuteBase {
    public:
        explicit SendWithReplyDBusMessageExecute(std::shared_ptr<DBusMessage> &&msg);

        void exec(DBusConnection *conn) override;
    };
}
