/**
* @file bluez.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

namespace pie::bluez {
    inline const auto *bus_name = "org.bluez";
    inline const char *service_org_bluez = "org.bluez";

    inline const char *path_org_bluez_hci0 = "/org/bluez/hci0";
}
