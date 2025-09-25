/**
* @file Service.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "Service.h"
#include "Characteristic.h"
#include "helper/service.h"
#include "pie/logging/console_helpers.h"
#include "pie/dbus/helper/dbus.h"

#include <vector>
#include <sstream>


namespace pie::bluez::gatt {
    struct ServiceData {
        std::string uuid{};
        std::string path{};
        std::string iface{};
        bool is_primary{true};
        std::vector<std::weak_ptr<Characteristic> > characteristics{};
        std::shared_ptr<pie::dbus::DBus> dbus;
        std::shared_ptr<pie::Logger> logger;
    };
} // pie::bluez::gatt

namespace {
    int id{0};
    inline const std::string TAG{"gatt::Service"};
}

namespace pie::bluez::gatt {
    Service::Service(std::string uuid, const std::string &base_path, bool is_primary,
                     const std::shared_ptr<pie::dbus::DBus> &dbus, const std::shared_ptr<pie::Logger> &logger) {
        data = std::make_shared<ServiceData>();
        data->uuid = std::move(uuid);
        std::stringstream ss{};
        ss << base_path << "/service" << id++;
        data->path = ss.str();
        data->iface = std::string(pie::bluez::gatt::service::iface);
        data->is_primary = is_primary;
        data->dbus = dbus;
        data->logger = logger;
    }

    Service::~Service() {
        std::stringstream ss{};
        ss << "Service::~Service()[";
        ss << "uuid: " << data->uuid;
        ss << ", path: " << data->path << "]";
        pie::logger::log_if_debug(data->logger, LogLevel::Trace, ss.str());
    }

    const std::string &Service::path() const {
        return data->path;
    }

    void Service::add_characteristic(std::weak_ptr<Characteristic> characteristic) {
        data->characteristics.emplace_back(characteristic);
    }

    void Service::get_managed_objects(DBusMessageIter *iter) {
        // Service entry: {oa{sa{sv}}}
        DBusMessageIter srv_iter;
        //"oa{sa{sv}}"
        dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, nullptr, &srv_iter);

        auto path = data->path.c_str();
        dbus_message_iter_append_basic(&srv_iter, DBUS_TYPE_OBJECT_PATH, &path);

        DBusMessageIter arr_iter;
        dbus_message_iter_open_container(&srv_iter, DBUS_TYPE_ARRAY, "{sa{sv}}", &arr_iter);

        DBusMessageIter dict_iter;
        // "sa{sv}"
        dbus_message_iter_open_container(&arr_iter, DBUS_TYPE_DICT_ENTRY, nullptr, &dict_iter);

        auto iface = data->iface.c_str();
        dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &iface);

        DBusMessageIter props_iter;
        dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_ARRAY, "{sv}", &props_iter);
        pie::dbus::message_append_dict_entry(
            &props_iter,
            pie::bluez::gatt::service::to_string(service::Property::UUID),
            data->uuid);

        pie::dbus::message_append_dict_entry(
            &props_iter,
            pie::bluez::gatt::service::to_string(
                pie::bluez::gatt::service::Property::Primary),
            data->is_primary);

        std::vector<std::string> chr_paths{};
        for (const auto &chr_weak: data->characteristics) {
            if (auto chr = chr_weak.lock())
                chr_paths.emplace_back(chr->path());
        }

        pie::dbus::message_append_dict_entry_objects(
            &props_iter,
            pie::bluez::gatt::service::to_string(
                service::Property::Characteristics),
            chr_paths);


        dbus_message_iter_close_container(&dict_iter, &props_iter);
        dbus_message_iter_close_container(&arr_iter, &dict_iter);
        dbus_message_iter_close_container(&srv_iter, &arr_iter);
        dbus_message_iter_close_container(iter, &srv_iter);

        for (const auto &chr_weak: data->characteristics) {
            if (auto chr = chr_weak.lock())
                chr->get_managed_objects(iter);
        }
    }
}
