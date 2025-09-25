/**
* @file Exception.h
* @author Ilija Poznic
* @date 2025
*/
#pragma once

#include <stdexcept>

namespace pie::bluez::gatt {
    class Exception : public std::runtime_error {
    public:
        explicit Exception(const std::string &msg);

        explicit Exception(const char *msg);
    };
} // pie::bluez::gatt
