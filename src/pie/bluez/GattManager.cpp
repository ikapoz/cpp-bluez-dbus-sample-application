/**
* @file GattManager.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "GattManager.h"

#include "pie/bluez/gatt/Exception.h"
#include "pie/bluez/gatt/helper/manager.h"
#include "pie/bluez/helper/bluez.h"

namespace {
    inline const std::string TAG{"GattManager"};
}

namespace pie::bluez {
    struct GattManagerData {
        std::string iface;
        std::string path;

        std::shared_ptr<Logger> logger;
        std::shared_ptr<pie::dbus::DBus> dbus;
    };

    GattManager::GattManager(
        const std::string &path,
        const std::shared_ptr<pie::dbus::DBus> &dbus,
        const std::shared_ptr<pie::Logger> &logger) {
        data = std::make_shared<GattManagerData>();
        data->iface = pie::bluez::gatt::manager::iface;
        data->path = path;
        data->dbus = dbus;
        data->logger = logger;
    }

    GattManager::~GattManager() {
        pie::logger::log_if_debug(
            data->logger, LogLevel::Trace, "GattManager::~GattManager()");
    }

    void GattManager::register_application(std::string const &path) const {
        std::string bus_name = pie::bluez::bus_name;
        std::string method = pie::bluez::gatt::manager::to_string(
            pie::bluez::gatt::manager::Methods::RegisterApplication);
        auto msg = data->dbus->new_message(
            bus_name,
            data->path,
            data->iface,
            method);

        DBusMessageIter iter{nullptr};
        dbus_message_iter_init_append(msg.get(), &iter);
        auto p_path = path.c_str();
        dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH, &p_path);
        DBusMessageIter array_iter{nullptr};
        dbus_message_iter_open_container(&iter,
                                         DBUS_TYPE_ARRAY, "{sv}", &array_iter);
        dbus_message_iter_close_container(&iter, &array_iter);
        dbus_message_iter_init_closed(&iter);

        auto result = data->dbus->send(std::move(msg));
        if (result.code != dbus::DBusResultCode::Success) {
            std::stringstream ss{};
            ss << "Failed to register application " << path;
            ss << ", error: " << result.error;
            throw pie::bluez::gatt::Exception(ss.str());
        }
    }

    void GattManager::unregister_application(std::string const &path) const {
        std::string bus_name = pie::bluez::bus_name;
        std::string method = pie::bluez::gatt::manager::to_string(
            pie::bluez::gatt::manager::Methods::UnregisterApplication);
        auto msg = data->dbus->new_message(
            bus_name,
            data->path,
            data->iface,
            method);

        DBusMessageIter iter{nullptr};
        dbus_message_iter_init_append(msg.get(), &iter);
        auto p_path = path.c_str();
        dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH, &p_path);
        auto result = data->dbus->send(std::move(msg));
        if (result.code != dbus::DBusResultCode::Success) {
            std::stringstream ss{};
            ss << "Failed to un-register application " << path;
            ss << ", error: " << result.error;
            throw pie::bluez::gatt::Exception(ss.str());
        }
    }
} // pie
