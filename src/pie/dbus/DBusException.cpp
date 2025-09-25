/**
* @file DBusException.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "DBusException.h"

namespace pie::dbus {
    DBusException::DBusException(const char *message) : std::logic_error(message) {
    }

    DBusException::DBusException(const std::string &message) : std::logic_error(message) {
    }



}
