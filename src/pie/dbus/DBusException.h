/**
* @file DBusException.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include "DBus.h"

#include <stdexcept>

namespace pie::dbus {
    class DBusException : public std::logic_error {
    public:
        explicit DBusException(const char *message);

        explicit DBusException(const std::string &message);
    };
}
