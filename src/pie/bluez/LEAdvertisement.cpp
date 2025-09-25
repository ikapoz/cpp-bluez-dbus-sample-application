/**
* @file LEAdvertisement.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "LEAdvertisement.h"
#include "pie/bluez/helper/le_advertisement.h"
#include "pie/dbus/helper/dbus.h"

#include <pie/logging/console_helpers.h>

namespace pie::bluez {
    struct LEAdvertisementData {
        std::string iface{pie::bluez::le_advertisement::iface};
        std::string path;
        std::shared_ptr<pie::dbus::DBus> dbus;
        std::shared_ptr<Logger> logger;

        // props
        LEAdvertisementType type{LEAdvertisementType::Peripheral};
        std::vector<std::string> uuids{};
        std::string name{};
    };
}

namespace {
    inline const std::string TAG = "LEAdvertisement";

    void append_properties(const std::shared_ptr<pie::bluez::LEAdvertisementData> &data,
                           const std::shared_ptr<DBusMessage> &reply,
                           DBusMessageIter *iter) {
        DBusMessageIter props_iter{nullptr};
        dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY, "{sv}", &props_iter);

        // Type
        {
            auto value = pie::bluez::le_advertisement::to_string(data->type);
            auto prop_name = pie::bluez::le_advertisement::to_string(
                pie::bluez::le_advertisement::Property::Type);
            pie::dbus::message_append_dict_entry(&props_iter, prop_name, value);
        }

        // uuids
        {
            auto value = data->uuids;
            auto prop_name = pie::bluez::le_advertisement::to_string(
                pie::bluez::le_advertisement::Property::ServiceUUIDs);
            pie::dbus::message_append_dict_entry(&props_iter, prop_name, value);
        }

        // name
        {
            auto value = data->name;
            auto prop_name = pie::bluez::le_advertisement::to_string(
                pie::bluez::le_advertisement::Property::LocalName);
            pie::dbus::message_append_dict_entry(&props_iter, prop_name, value);
        }


        dbus_message_iter_close_container(iter, &props_iter);
    }

    DBusHandlerResult on_message_properties_get_all(const pie::dbus::DBusMessageInfo &msg_info,
                                                    const std::shared_ptr<DBusMessage> &message,
                                                    const std::shared_ptr<pie::bluez::LEAdvertisementData> &data) {
        if (pie::dbus::properties::is_method(
            msg_info, data->path,
            pie::dbus::properties::Methods::GetAll)) {
            std::stringstream ss{};
            ss << "on_message: path: " << msg_info.path;;
            ss << ", method: Properties_GetAll";
            pie::logger::log_if_debug(data->logger, TAG, pie::LogLevel::Trace,
                                      ss.str());
            auto [success, reply_msg] = pie::dbus::message_new_method_return(data->logger, message);
            if (!success)
                return DBUS_HANDLER_RESULT_NEED_MEMORY;

            DBusMessageIter iter{nullptr};
            dbus_message_iter_init_append(reply_msg.get(), &iter);
            append_properties(data, reply_msg, &iter);
            dbus_message_iter_init_closed(&iter);

            auto result = data->dbus->reply(std::move(reply_msg));
            if (result.code != pie::dbus::DBusResultCode::Success) {
                ss << ", error: " << result.error;
                pie::logger::log_if_debug(data->logger, TAG, pie::LogLevel::Warning, ss.str());
                return DBUS_HANDLER_RESULT_NEED_MEMORY;
            }

            return DBUS_HANDLER_RESULT_HANDLED;
        }

        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    DBusHandlerResult on_message_obj_mng_get_mng_objs(const pie::dbus::DBusMessageInfo &msg_info,
                                                      const std::shared_ptr<DBusMessage> &message,
                                                      const std::shared_ptr<pie::bluez::LEAdvertisementData> &data) {
        if (pie::dbus::object_manager::is_method(
            msg_info, data->path,
            pie::dbus::object_manager::Methods::GetManagedObject)) {
            std::stringstream ss{};
            ss << "on_message: path: " << msg_info.path;;
            ss << ", method: ObjectManager_GetManagedObject";
            pie::logger::log_if_debug(data->logger, TAG, pie::LogLevel::Trace,
                                      ss.str());

            auto [success, reply_msg] = pie::dbus::message_new_method_return(data->logger, message);
            if (!success)
                return DBUS_HANDLER_RESULT_NEED_MEMORY;

            DBusMessageIter iter{nullptr};
            dbus_message_iter_init_append(reply_msg.get(), &iter);
            DBusMessageIter dict_iter{nullptr};
            // {oa{sa{sv}}}
            dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
                                             "{oa{sa{sv}}}", &dict_iter);
            // get managed objects
            DBusMessageIter srv_iter{nullptr};
            // "os{sa{sv}}
            dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY,
                                             nullptr, &srv_iter);

            auto path = data->path.c_str();
            dbus_message_iter_append_basic(&srv_iter, DBUS_TYPE_OBJECT_PATH, &path);

            DBusMessageIter arr_iter{nullptr};
            dbus_message_iter_open_container(&srv_iter, DBUS_TYPE_ARRAY,
                                             "{sa{sv}}", &arr_iter);

            DBusMessageIter if_dict_iter{nullptr};
            dbus_message_iter_open_container(&arr_iter, DBUS_TYPE_DICT_ENTRY,
                                             nullptr, &if_dict_iter);

            auto iface = data->iface.c_str();
            dbus_message_iter_append_basic(&if_dict_iter, DBUS_TYPE_STRING, &iface);

            append_properties(data, reply_msg, &if_dict_iter);

            dbus_message_iter_close_container(&arr_iter, &if_dict_iter);
            dbus_message_iter_close_container(&srv_iter, &arr_iter);
            dbus_message_iter_close_container(&dict_iter, &srv_iter);
            dbus_message_iter_close_container(&iter, &dict_iter);
            dbus_message_iter_init_closed(&iter);

            auto result = data->dbus->reply(std::move(reply_msg));
            if (result.code != pie::dbus::DBusResultCode::Success) {
                ss << ", error: " << result.error;
                pie::logger::log_if_debug(data->logger, TAG, pie::LogLevel::Warning, ss.str());
                return DBUS_HANDLER_RESULT_NEED_MEMORY;
            }

            return DBUS_HANDLER_RESULT_HANDLED;
        }

        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
}


namespace pie::bluez {
    LEAdvertisement::LEAdvertisement(std::string path, std::shared_ptr<pie::dbus::DBus> dbus,
                                     std::shared_ptr<pie::Logger> logger) {
        data = std::make_shared<LEAdvertisementData>();
        data->path = std::move(path);
        data->dbus = std::move(dbus);
        data->logger = std::move(logger);
    }

    LEAdvertisement::~LEAdvertisement() {
        pie::logger::log_if_debug(data->logger, LogLevel::Trace, "LEAdvertisement::~LEAdvertisement()");
    }

    void LEAdvertisement::register_advertisement(std::shared_ptr<DBusMessage> msg) {
        auto msg_p = msg.get();
        DBusMessageIter arg_iter{nullptr};
        dbus_message_iter_init_append(msg_p, &arg_iter);
        auto p_path = data->path.c_str();
        dbus_message_iter_append_basic(&arg_iter, DBUS_TYPE_OBJECT_PATH, &p_path);

        append_properties(data, msg, &arg_iter);
        dbus_message_iter_init_closed(&arg_iter);
    }

    void LEAdvertisement::unregister_advertisement(std::shared_ptr<DBusMessage> msg) {
        auto msg_p = msg.get();
        DBusMessageIter arg_iter{nullptr};
        dbus_message_iter_init_append(msg_p, &arg_iter);
        auto p_path = data->path.c_str();
        dbus_message_iter_append_basic(&arg_iter, DBUS_TYPE_OBJECT_PATH, &p_path);
        dbus_message_iter_init_closed(&arg_iter);
    }

    LEAdvertisementType LEAdvertisement::type() {
        return data->type;
    }

    void LEAdvertisement::type(LEAdvertisementType type) {
        data->type = type;
        // TODO publish changes
    }

    std::vector<std::string> LEAdvertisement::service_uuids() {
        return data->uuids;
    }

    void LEAdvertisement::service_uuids(std::vector<std::string> uuids) {
        data->uuids = std::move(uuids);
        // TODO publish changes
    }

    std::string LEAdvertisement::name() {
        return data->name;
    }

    void LEAdvertisement::name(std::string set) {
        data->name = set;
    }


    DBusHandlerResult LEAdvertisement::on_message(const dbus::DBusMessageInfo &msg_info,
                                                  std::shared_ptr<DBusMessage> message) {
        try {
            if (msg_info.path == data->path) {
                auto result = on_message_properties_get_all(msg_info, message, data);
                if (result != DBUS_HANDLER_RESULT_NOT_YET_HANDLED)
                    return result;

                result = on_message_obj_mng_get_mng_objs(msg_info, message, data);
                if (result != DBUS_HANDLER_RESULT_NOT_YET_HANDLED)
                    return result;
            }
        } catch (const std::exception &e) {
            std::stringstream ss;
            ss << "onmessage error: " << e.what();
            pie::logger::log(data->logger, TAG, LogLevel::Error, ss.str());
            return DBUS_HANDLER_RESULT_NEED_MEMORY;
        }

        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
} // pie::bluez
