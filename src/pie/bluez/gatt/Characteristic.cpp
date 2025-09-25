/**
* @file Characteristic.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "Characteristic.h"
#include "Service.h"
#include "helper/characteristic.h"
#include "pie/dbus/helper/dbus.h"

#include <pie/logging/console_helpers.h>

#include "helper/service.h"

namespace pie::bluez::gatt {
    struct CharacteristicData {
        std::string uuid{};
        std::string path{};
        std::string iface{};
        std::weak_ptr<Service> service;
        std::shared_ptr<pie::dbus::DBus> dbus;
        std::shared_ptr<pie::Logger> logger;
        std::vector<std::string> flags{};
        std::vector<uint8_t> value{};
        std::weak_ptr<OnValueChanged> subscriber;
    };
}

namespace {
    int id{0};
    inline const std::string TAG{"gatt::Characteristic"};
}

namespace pie::bluez::gatt {
    Characteristic::Characteristic(const std::string &uuid,
                                   const std::weak_ptr<Service> &service,
                                   std::vector<pie::bluez::gatt::characteristic::Flag> &&flags,
                                   const std::shared_ptr<OnValueChanged> &subscriber,
                                   const std::shared_ptr<pie::dbus::DBus> &dbus,
                                   const std::shared_ptr<pie::Logger> &logger) {
        data = std::make_shared<CharacteristicData>();
        data->service = service;
        data->subscriber = subscriber;
        data->dbus = dbus;
        data->logger = logger;
        data->uuid = uuid;
        std::stringstream ss{};
        if (auto p_service = data->service.lock())
            ss << p_service->path() << "/characteristic" << id++;
        data->path = ss.str();
        data->iface = pie::bluez::gatt::characteristic::iface;
        std::vector<std::string> flags_as_strings{};
        flags_as_strings.reserve(flags.size());
        for (const auto &flag: flags)
            flags_as_strings.emplace_back(pie::bluez::gatt::characteristic::to_string(flag));

        data->flags = flags_as_strings;
    }

    Characteristic::~Characteristic() {
        std::stringstream ss;
        ss << "Characteristic::~Characteristic()[";
        ss << "uuid: " << data->uuid;
        ss << ", path: " << data->path << "]";
        pie::logger::log_if_debug(data->logger, TAG, LogLevel::Trace, ss.str());
    }

    const std::string &Characteristic::path() const {
        return data->path;
    }

    const std::string &Characteristic::uuid() const {
        return data->uuid;
    }

    void Characteristic::get_managed_objects(DBusMessageIter *iter) {
        // Characteristic entry: {oa{sa{sa}}}
        DBusMessageIter sub_iter;
        // "oa{sa{sv}}"
        dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, nullptr, &sub_iter);

        auto path = data->path.c_str();
        dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_OBJECT_PATH, &path);

        DBusMessageIter arr_iter;
        dbus_message_iter_open_container(&sub_iter, DBUS_TYPE_ARRAY, "{sa{sv}}", &arr_iter);

        DBusMessageIter dict_iter;
        dbus_message_iter_open_container(&arr_iter, DBUS_TYPE_DICT_ENTRY, nullptr, &dict_iter);

        auto iface = data->iface.c_str();
        dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &iface);

        // add properties as variant values
        DBusMessageIter props_iter;
        dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_ARRAY, "{sv}", &props_iter);

        // service
        if (auto service = data->service.lock()) {
            auto service_path = service->path();
            pie::dbus::message_append_dict_entry_object(
                &props_iter,
                pie::bluez::gatt::characteristic::to_string(
                    characteristic::Property::Service),

                service_path);
        }

        // UUID
        pie::dbus::message_append_dict_entry(
            &props_iter,
            pie::bluez::gatt::characteristic::to_string(
                characteristic::Property::UUID),
            data->uuid);

        // Flags
        pie::dbus::message_append_dict_entry(
            &props_iter,
            pie::bluez::gatt::characteristic::to_string(
                characteristic::Property::Flags),
            data->flags);

        // Descriptor array of objects
        std::vector<std::string> descriptor_uuids{};
        // TODO add descriptors UUIDs to the list
        pie::dbus::message_append_dict_entry_objects(
            &props_iter,
            pie::bluez::gatt::characteristic::to_string(
                characteristic::Property::Descriptors),
            descriptor_uuids);


        dbus_message_iter_close_container(&dict_iter, &props_iter);
        dbus_message_iter_close_container(&arr_iter, &dict_iter);
        dbus_message_iter_close_container(&sub_iter, &arr_iter);
        dbus_message_iter_close_container(iter, &sub_iter);
    }

    DBusHandlerResult Characteristic::on_message(
        const dbus::DBusMessageInfo &msg_info, std::shared_ptr<DBusMessage> message) {
        if (msg_info.path != data->path)
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

        if (pie::bluez::gatt::characteristic::is_method(msg_info, data->path,
                                                        pie::bluez::gatt::characteristic::Methods::WriteValue)) {
            pie::logger::log_if_debug(data->logger, TAG, LogLevel::Trace,
                                      "on_message: Characteristic_WriteValue");

            DBusMessageIter iter{nullptr};
            dbus_message_iter_init(message.get(), &iter);
            data->value = pie::dbus::message_get_bytes(&iter);

            if (auto subscriber = data->subscriber.lock())
                subscriber->on_value_changed(data->path, data->value);

            dbus_message_iter_init_closed(&iter);
            return DBUS_HANDLER_RESULT_HANDLED;
        }

        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
}
