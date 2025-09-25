/**
* @file SendDBusMessageExecute.h
* @author Ilija Poznic
* @date 2025
*/


#pragma once

#include "DBusMessageExecuteBase.h"

namespace pie::dbus {
    class SendDBusMessageExecute : public DBusMessageExecuteBase {
    public:
        explicit SendDBusMessageExecute(std::shared_ptr<DBusMessage> &&msg);

        void exec(DBusConnection *conn) override;
    };
} // pie
