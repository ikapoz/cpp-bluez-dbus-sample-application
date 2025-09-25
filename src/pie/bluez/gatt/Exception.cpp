/**
* @file Exception.cpp
* @author Ilija Poznic
* @date 2025
*/


#include "Exception.h"

namespace pie::bluez::gatt {
    Exception::Exception(const char *msg) : std::runtime_error(msg) {
    }

    Exception::Exception(const std::string &msg) : std::runtime_error(msg) {
    }
} // pie::bluez::gatt
