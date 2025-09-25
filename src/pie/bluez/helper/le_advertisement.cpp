/**
* @file le_advertisement.cpp
* @author Ilija Poznic
* @date 2025
*/


#include "le_advertisement.h"

// #include "pie/bluez/helper.h"
#include "pie/logging/console_helpers.h"

#include <cstring>

namespace {
    std::shared_ptr<pie::bluez::LEAdvertisement> le_advertisement_hci0;
}

namespace pie::bluez::le_advertisement {
    // std::shared_ptr<pie::bluez::LEAdvertisement> hci0(const std::shared_ptr<pie::Logger> &logger) {
    //     if (le_advertisement_hci0 == nullptr)
    //         le_advertisement_hci0 = std::make_shared<pie::bluez::LEAdvertisement>(
    //             pie::bluez::path_org_bluez_hci0, logger);
    //
    //     return le_advertisement_hci0;
    // }

    bool is_interface(const pie::dbus::DBusMessageInfo &msg_info) {
        return msg_info.iface == iface;
    }

    std::string to_string(pie::bluez::LEAdvertisementType type) {
        switch (type) {
            case LEAdvertisementType::Broadcast:
                return "broadcast";
            case LEAdvertisementType::Peripheral:
                return "peripheral";
            case LEAdvertisementType::Unknown:
                return "Unknown";
            default:
                break;
        }

        return "Unknown";
    }

    Property to_property(const char *property_name) {
        if (strcmp(property_name, "Type") == 0)
            return Property::Type;

        if (strcmp(property_name, "ServiceUUIDs") == 0)
            return Property::ServiceUUIDs;

        if (strcmp(property_name, "ManufacturerData") == 0)
            return Property::ManufacturerData;

        if (strcmp(property_name, "SolicitUUIDs") == 0)
            return Property::SolicitUUIDs;

        if (strcmp(property_name, "ServiceData") == 0)
            return Property::ServiceData;

        if (strcmp(property_name, "Data") == 0)
            return Property::Data;

        if (strcmp(property_name, "Includes") == 0)
            return Property::Includes;

        if (strcmp(property_name, "Duration") == 0)
            return Property::Duration;

        if (strcmp(property_name, "Timeout") == 0)
            return Property::Timeout;

        return Property::Unknown;
    }

    std::string to_string(Property property) {
        switch (property) {
            case Property::Type:
                return "Type";
            case Property::ServiceUUIDs:
                return "ServiceUUIDs";
            case Property::ManufacturerData:
                return "ManufacturerData";
            case Property::SolicitUUIDs:
                return "SolicitUUIDs";
            case Property::ServiceData:
                return "ServiceData";
            case Property::Data:
                return "Data";
            case Property::Includes:
                return "Includes";
            case Property::LocalName:
                return "LocalName";
            case Property::Duration:
                return "Duration";
            case Property::Timeout:
                return "Timeout";
            default:
                return "Unknown";
        }
    }
} // pie::bluez::le_advertisement
