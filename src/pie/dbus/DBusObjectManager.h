/**
* @file DBusObjectManager.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once
#include "DBusOnMessage.h"

namespace pie::dbus {
    class DBusObjectManager {
    public:
        virtual ~DBusObjectManager() = default;

        virtual void get_managed_objects(DBusMessageIter *iter) = 0;
    };
}
