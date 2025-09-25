/**
* @file characteristic.cpp
 * @author Ilija Poznic
 * @date 2025
 */

#include "characteristic.h"

namespace pie::bluez::gatt::characteristic {
    bool is_interface(const pie::dbus::DBusMessageInfo &msg_info) {
        return msg_info.iface == iface;
    }

    std::string to_string(Property property) {
        switch (property) {
            case Property::UUID:
                return "UUID";
            case Property::Service:
                return "Service";
            case Property::Flags:
                return "Flags";
            case Property::Descriptors:
                return "Descriptors";
            case Property::Value:
                return "Value";
            default:
                return "Unknown";
        }
    }

    std::string to_string(Flag flag) {
        switch (flag) {
            case Flag::Broadcast:
                return "broadcast";
            case Flag::Read:
                return "read";
            case Flag::WriteWithoutResponse:
                return "write-without-response";
            case Flag::Write:
                return "write";
            case Flag::Notify:
                return "notify";
            case Flag::Indicate:
                return "indicate";
            default:
                return "Unknown";
        }
    }

    std::string to_string(Methods method) {
        switch (method) {
            case Methods::ReadValue:
                return "ReadValue";
            case Methods::WriteValue:
                return "WriteValue";
            default:
                return "Unknown";
        }
    }

    bool is_method(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Methods method) {
        if (msg_info.iface == iface &&
            msg_info.path == path &&
            msg_info.member == to_string(method))
            return true;

        return false;
    }
} // pie::bluez::gatt::characteristic
