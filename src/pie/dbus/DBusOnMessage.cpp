/**
* @file DBusOnMessage.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "DBusOnMessage.h"

namespace pie::dbus {
    std::ostream &operator<<(std::ostream &os, DBusMessageType type) {
        switch (type) {
            case DBusMessageType::Invalid:
                return os << "Invalid";
            case DBusMessageType::MethodCall:
                return os << "MethodCall";
            case DBusMessageType::MethodReturn:
                return os << "MethodReturn";
            case DBusMessageType::Signal:
                return os << "Signal";
            case DBusMessageType::Unknown:
                return os << "Unknown";
        }

        return os;
    }

    std::ostream &operator<<(std::ostream &os, DBusMessageInfo &info) {
        os << "{type: " << info.type;
        if (info.type != DBusMessageType::MethodReturn) {
            os << ", destination: " << info.destination;
            os << ", path: " << info.path;
            os << ", iface: " << info.iface;
            os << ", member: " << info.member;
        }

        if (info.reply_serial.has_value())
            os << ", reply_serial: " << info.reply_serial.value();

        os << ", serial: " << info.serial;
        os << "}";
        return os;
    }
}
