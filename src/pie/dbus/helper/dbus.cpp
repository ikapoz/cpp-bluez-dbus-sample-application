/**
* @file dbus.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "dbus.h"
#include "pie/dbus/DBusException.h"

#include <chrono>
#include <sstream>
#include <cstring>

namespace pie::dbus {
    pie::dbus::DBusResult parse(DBusError *error) {
        if (dbus_error_is_set(error)) {
            std::stringstream ss;
            ss << "error name: " << error->name;
            ss << ", message: " << error->message;
            if ((strcmp(error->name, DBUS_ERROR_TIMEOUT) == 0) ||
                (strcmp(error->name, DBUS_ERROR_TIMED_OUT) == 0)) {
                return {
                    .code = pie::dbus::DBusResultCode::E_CMD_Timeout,
                    .error = ss.str()
                };
            }

            return {
                .code = pie::dbus::DBusResultCode::Error,
                .error = ss.str()
            };
        }

        return {};
    }

    pie::dbus::DBusMessageInfo get_message_info(std::shared_ptr<DBusMessage> msg) {
        auto msg_p = msg.get();
        auto msg_type = dbus_message_get_type(msg_p);
        pie::dbus::DBusMessageInfo msg_info{};
        msg_info.type = pie::dbus::message_get_type(msg_type);
        auto destination = dbus_message_get_destination(msg_p);
        if (destination)
            msg_info.destination = destination;

        auto path = dbus_message_get_path(msg_p);
        if (path)
            msg_info.path = path;

        auto iface = dbus_message_get_interface(msg_p);
        if (iface)
            msg_info.iface = iface;

        auto member = dbus_message_get_member(msg_p);
        if (member)
            msg_info.member = member;

        msg_info.serial = dbus_message_get_serial(msg_p);
        if (msg_info.type == DBusMessageType::MethodReturn) {
            auto reply_serial = dbus_message_get_reply_serial(msg_p);
            msg_info.serial = reply_serial;
        }

        return msg_info;
    }

    std::string get_message_info(const pie::dbus::DBusMessageInfo &msg_info) {
        std::stringstream ss;
        auto msg_type_as_string = pie::dbus::message_type_as_string(msg_info.type);
        ss << "msg received [type: " << msg_type_as_string;
        if (msg_info.type != DBusMessageType::MethodReturn) {
            if (!msg_info.path.empty())
                ss << ", path: " << msg_info.path;

            if (!msg_info.iface.empty())
                ss << ", iface: " << msg_info.iface;

            if (!msg_info.member.empty())
                ss << ", member: " << msg_info.member;
        }

        if (msg_info.reply_serial.has_value())
            ss << ", serial_return: " << msg_info.reply_serial.value();

        ss << ", serial: " << msg_info.serial;
        ss << "]";
        return ss.str();
    }

    pie::dbus::DBusMessageResult connection_send_with_reply_and_block(DBusConnection *connection,
                                                                      DBusMessage *msg,
                                                                      std::chrono::milliseconds timeout) {
        DBusError err{};
        DBusMessageResult result{};
        auto msg_reply_p = dbus_connection_send_with_reply_and_block(connection, msg, timeout.count(), &err);
        if (msg_reply_p == nullptr) {
            auto error_result = parse(&err);
            dbus_error_free(&err);

            result.code = error_result.code;
            result.error = error_result.error;
            return result;
        }

        if (dbus_set_error_from_message(&err, msg_reply_p) != 0) {
            auto error_result = parse(&err);
            dbus_error_free(&err);
            std::stringstream ss;
            result.code = error_result.code;
            result.error = error_result.error;
            dbus_message_unref(msg_reply_p);
            return result;
        }

        std::shared_ptr<DBusMessage> msg_reply = std::shared_ptr<DBusMessage>(msg_reply_p, [](DBusMessage *msg) {
            dbus_message_unref(msg);
        });
        result.message = std::move(msg_reply);
        return result;
    }

    void throw_dbus_exception_if_error(const DBusResult &result) {
        if (result.code != pie::dbus::DBusResultCode::Success)
            throw pie::dbus::DBusException(result.error);
    }

    std::string message_type_as_string(DBusMessageType dbus_message_type) {
        switch (dbus_message_type) {
            case DBusMessageType::Invalid:
                return "Invalid";
            case DBusMessageType::MethodCall:
                return "MethodCall";
            case DBusMessageType::MethodReturn:
                return "MethodReturn";
            case DBusMessageType::Signal:
                return "Signal";
            case DBusMessageType::Unknown:
                return "Unknown";
            default:
                return "Unknown";
        }
    }

    DBusMessageType message_get_type(int dbus_message_type) {
        switch (dbus_message_type) {
            case 0:
                return DBusMessageType::Invalid;
            case 1:
                return DBusMessageType::MethodCall;
            case 2:
                return DBusMessageType::MethodReturn;
            case 4:
                return DBusMessageType::Signal;
            default:
                return DBusMessageType::Unknown;
        }
    }

    std::tuple<bool, std::shared_ptr<DBusMessage> > message_new_method_return(
        const std::shared_ptr<pie::Logger> &logger,
        const std::shared_ptr<DBusMessage> &message) {
        auto reply_p = dbus_message_new_method_return(message.get());
        if (!reply_p) {
            logger->log(pie::LogLevel::Error, "Failed to create DBus message. No memory left");
            return {false, nullptr};
        }

        std::shared_ptr<DBusMessage> reply(reply_p, [](DBusMessage *msg) {
            if (msg)
                dbus_message_unref(msg);
        });

        return std::make_tuple(true, reply);
    }

    void message_append_dict_entry(DBusMessageIter *iter, const std::string &property_name, const std::string &value) {
        DBusMessageIter dict_iter{nullptr};
        dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, nullptr, &dict_iter);
        auto p_property_name = property_name.c_str();
        dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &p_property_name);

        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_open_container(&dict_iter,
                                         DBUS_TYPE_VARIANT,
                                         DBUS_TYPE_STRING_AS_STRING,
                                         &var_iter);
        auto p_value = value.c_str();
        dbus_message_iter_append_basic(&var_iter, DBUS_TYPE_STRING, &p_value);

        dbus_message_iter_close_container(&dict_iter, &var_iter);
        dbus_message_iter_close_container(iter, &dict_iter);
    }

    void message_append_dict_entry(DBusMessageIter *iter, const std::string &property_name, bool value) {
        DBusMessageIter dict_iter{nullptr};
        dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, nullptr, &dict_iter);
        auto p_property_name = property_name.c_str();
        dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &p_property_name);

        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_open_container(&dict_iter,
                                         DBUS_TYPE_VARIANT,
                                         DBUS_TYPE_BOOLEAN_AS_STRING,
                                         &var_iter);

        dbus_bool_t value_as_bool = value ? 1 : 0;
        dbus_message_iter_append_basic(&var_iter, DBUS_TYPE_BOOLEAN, &value_as_bool);

        dbus_message_iter_close_container(&dict_iter, &var_iter);
        dbus_message_iter_close_container(iter, &dict_iter);
    }

    void message_append_dict_entry(DBusMessageIter *iter, const std::string &property_name,
                                   const std::vector<std::string> &values) {
        DBusMessageIter dict_iter{nullptr};
        dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, nullptr, &dict_iter);
        auto p_property_name = property_name.c_str();
        dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &p_property_name);

        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_open_container(&dict_iter,
                                         DBUS_TYPE_VARIANT,
                                         "as",
                                         &var_iter);

        DBusMessageIter arr_iter{nullptr};
        dbus_message_iter_open_container(&var_iter, DBUS_TYPE_ARRAY,
                                         DBUS_TYPE_STRING_AS_STRING, &arr_iter);

        for (const auto &value: values) {
            auto v = value.data();
            dbus_message_iter_append_basic(&arr_iter, DBUS_TYPE_STRING, &v);
        }


        dbus_message_iter_close_container(&var_iter, &arr_iter);
        dbus_message_iter_close_container(&dict_iter, &var_iter);
        dbus_message_iter_close_container(iter, &dict_iter);
    }

    void message_append_dict_entry(DBusMessageIter *iter, const std::string &property_name,
                                   const std::vector<uint8_t> &values) {
        DBusMessageIter dict_iter{nullptr};
        dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, nullptr, &dict_iter);
        auto p_property_name = property_name.c_str();
        dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &p_property_name);

        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_open_container(&dict_iter,
                                         DBUS_TYPE_VARIANT,
                                         "ay",
                                         &var_iter);

        DBusMessageIter arr_iter{nullptr};
        dbus_message_iter_open_container(&var_iter, DBUS_TYPE_ARRAY,
                                         DBUS_TYPE_BYTE_AS_STRING, &arr_iter);

        dbus_message_iter_append_fixed_array(&arr_iter, DBUS_TYPE_BYTE,
                                             values.data(), values.size());

        dbus_message_iter_close_container(&var_iter, &arr_iter);
        dbus_message_iter_close_container(&dict_iter, &var_iter);
        dbus_message_iter_close_container(iter, &dict_iter);
    }

    void message_append_dict_entry_objects(DBusMessageIter *iter,
                                           const std::string &property_name,
                                           const std::vector<std::string> &objects) {
        DBusMessageIter dict_iter{nullptr};
        dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, nullptr, &dict_iter);
        auto p_property_name = property_name.c_str();
        dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &p_property_name);


        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_VARIANT,
                                         "ao", &var_iter);

        DBusMessageIter arr_iter{nullptr};
        dbus_message_iter_open_container(&var_iter, DBUS_TYPE_ARRAY,
                                         DBUS_TYPE_OBJECT_PATH_AS_STRING, &arr_iter);

        for (const auto obj: objects) {
            auto v = obj.data();
            dbus_message_iter_append_basic(&arr_iter, DBUS_TYPE_OBJECT_PATH, &v);
        }


        dbus_message_iter_close_container(&var_iter, &arr_iter);
        dbus_message_iter_close_container(&dict_iter, &var_iter);
        dbus_message_iter_close_container(iter, &dict_iter);
    }


    void message_append_dict_entry_object(DBusMessageIter *iter,
                                          const std::string &property_name,
                                          const std::string &value) {
        DBusMessageIter dict_iter{nullptr};
        dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, nullptr, &dict_iter);
        auto p_property_name = property_name.c_str();
        dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &p_property_name);

        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_open_container(&dict_iter,
                                         DBUS_TYPE_VARIANT,
                                         DBUS_TYPE_OBJECT_PATH_AS_STRING,
                                         &var_iter);

        auto v = value.data();
        dbus_message_iter_append_basic(&var_iter, DBUS_TYPE_OBJECT_PATH, &v);

        dbus_message_iter_close_container(&dict_iter, &var_iter);
        dbus_message_iter_close_container(iter, &dict_iter);
    }

    void message_set_variant(DBusMessageIter *iter, const std::string &value) {
        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_open_container(iter,
                                         DBUS_TYPE_VARIANT,
                                         DBUS_TYPE_STRING_AS_STRING,
                                         &var_iter);
        auto v = value.data();
        dbus_message_iter_append_basic(&var_iter, DBUS_TYPE_STRING, &v);
        dbus_message_iter_close_container(iter, &var_iter);
    }

    void message_set_variant(DBusMessageIter *iter, bool value) {
        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_open_container(iter,
                                         DBUS_TYPE_VARIANT,
                                         DBUS_TYPE_BOOLEAN_AS_STRING,
                                         &var_iter);
        dbus_bool_t v = value ? TRUE : FALSE;

        dbus_message_iter_append_basic(&var_iter, DBUS_TYPE_BOOLEAN, &v);
        dbus_message_iter_close_container(iter, &var_iter);
    }


    std::string message_get_variant_as_string(DBusMessageIter *iter) {
        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_recurse(iter, &var_iter);
        auto type = dbus_message_iter_get_arg_type(&var_iter);
        if (type != DBUS_TYPE_STRING)
            throw std::logic_error("variant value is not string");

        char *value{nullptr};
        dbus_message_iter_get_basic(&var_iter, &value);
        dbus_message_iter_next(&var_iter);
        return std::string(value);
    }

    bool message_get_variant_as_boolean(DBusMessageIter *iter) {
        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_recurse(iter, &var_iter);
        auto type = dbus_message_iter_get_arg_type(&var_iter);
        if (type != DBUS_TYPE_BOOLEAN)
            throw std::logic_error("variant value is not boolean");

        dbus_bool_t value = TRUE;
        dbus_message_iter_get_basic(&var_iter, &value);
        dbus_message_iter_next(&var_iter);
        return value == TRUE;
    }

    std::string message_get_variant_as_object(DBusMessageIter *iter) {
        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_recurse(iter, &var_iter);
        auto type = dbus_message_iter_get_arg_type(&var_iter);
        if (type != DBUS_TYPE_OBJECT_PATH)
            throw std::logic_error("variant value is not object");

        char *value{nullptr};
        dbus_message_iter_get_basic(&var_iter, &value);
        dbus_message_iter_next(&var_iter);
        return std::string(value);
    }

    uint16_t message_get_variant_as_uint16(DBusMessageIter *iter) {
        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_recurse(iter, &var_iter);
        auto type = dbus_message_iter_get_arg_type(&var_iter);
        if (type != DBUS_TYPE_UINT16)
            throw std::logic_error("variant value is not uint16_t");

        uint16_t value{0};
        dbus_message_iter_get_basic(&var_iter, &value);
        dbus_message_iter_next(&var_iter);
        return value;
    }

    std::vector<uint8_t> message_get_bytes(DBusMessageIter *iter) {
        if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_ARRAY)
            throw std::logic_error("it is not array");

        auto cnt = dbus_message_iter_get_element_count(iter);
        DBusMessageIter arr_iter{nullptr};
        dbus_message_iter_recurse(iter, &arr_iter);
        auto type = dbus_message_iter_get_arg_type(&arr_iter);
        if (type != DBUS_TYPE_BYTE)
            throw std::logic_error("it is not byte array");

        std::vector<uint8_t> bytes{};
        uint8_t *p{nullptr};
        dbus_message_iter_get_fixed_array(&arr_iter, &p, &cnt);
        bytes.reserve(cnt);
        for (size_t i = 0; i < cnt; i++)
            bytes.push_back(p[i]);

        return bytes;
    }

    void message_variant_iter_next(DBusMessageIter *iter) {
        DBusMessageIter var_iter{nullptr};
        dbus_message_iter_recurse(iter, &var_iter);
        dbus_message_iter_next(&var_iter);
    }

    bool is_match(const pie::dbus::DBusMessageInfo &msg_info,
                  pie::dbus::DBusMessageType type, const std::string &path,
                  const std::string &iface, const std::string &member) {
        return (msg_info.type == type &&
                msg_info.path == path &&
                msg_info.iface == iface &&
                msg_info.member == member);
    }

    namespace object_manager {
        bool is_interface(const pie::dbus::DBusMessageInfo &msg_info) {
            return msg_info.iface == pie::dbus::object_manager::iface;
        }

        bool is_method(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Methods method) {
            std::string member;
            switch (method) {
                case Methods::GetManagedObject:
                    member = "GetManagedObjects";
                    break;
                default:
                    break;
            }

            return (msg_info.path == path &&
                    msg_info.iface == iface &&
                    msg_info.member == member);
        }

        bool is_signal(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Signals signal) {
            std::string member;
            switch (signal) {
                case Signals::InterfacesAdded:
                    member = "InterfacesAdded";
                    break;
                case Signals::InterfacesRemoved:
                    member = "InterfacesRemoved";
                default:
                    break;
            }

            return (msg_info.path == path &&
                    msg_info.iface == pie::dbus::properties::iface &&
                    msg_info.member == member);
        }
    }

    namespace properties {
        bool is_interface(const pie::dbus::DBusMessageInfo &msg_info) {
            return msg_info.iface == pie::dbus::properties::iface;
        }

        bool is_method(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Methods method) {
            std::string member;
            switch (method) {
                case Methods::Get:
                    member = "Get";
                    break;
                case Methods::Set:
                    member = "Set";
                    break;
                case Methods::GetAll:
                    member = "GetAll";;
                    break;
                default:
                    break;
            }

            return (msg_info.path == path &&
                    msg_info.iface == pie::dbus::properties::iface &&
                    msg_info.member == member);
        }

        // DBusMessage *message_new(const std::string &service, const std::string &path, Methods method) {
        //     std::string member;
        //     switch (method) {
        //         case Methods::Get:
        //             member = "Get";
        //             break;
        //         case Methods::Set:
        //             member = "Set";
        //             break;
        //         case Methods::GetAll:
        //             member = "GetAll";
        //             break;
        //     }
        //     auto p_member = member.c_str();
        //     auto p_service = service.c_str();
        //     auto p_path = path.c_str();
        //
        //     return dbus_message_new_method_call(p_service, p_path, iface, p_member);
        // }

        std::shared_ptr<DBusMessage> message_new_get_all(const std::string &service_bus_name, const std::string &path,
                                                         const std::string &for_interface) {
            auto member = "GetAll";
            auto msg_p = dbus_message_new_method_call(service_bus_name.c_str(), path.c_str(), iface, member);
            if (msg_p) {
                DBusMessageIter iter{nullptr};
                dbus_message_iter_init_append(msg_p, &iter);
                auto p_for_iface = for_interface.c_str();
                dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &p_for_iface);
            }

            std::shared_ptr<DBusMessage> msg(msg_p, [](DBusMessage *msg) {
                if (msg)
                    dbus_message_unref(msg);
            });

            return msg;
        }

        std::shared_ptr<DBusMessage> message_new_get(const std::string &service_bus_name, const std::string &path,
                                                     const std::string &for_interface,
                                                     const std::string &property_name) {
            auto member = "Get";
            auto msg_p = dbus_message_new_method_call(service_bus_name.c_str(), path.c_str(), iface, member);
            if (msg_p) {
                DBusMessageIter iter{nullptr};
                dbus_message_iter_init_append(msg_p, &iter);
                auto p_for_iface = for_interface.c_str();
                dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &p_for_iface);
                auto p_prop_name = property_name.c_str();
                dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &p_prop_name);
            }

            std::shared_ptr<DBusMessage> msg(msg_p, [](DBusMessage *msg) {
                if (msg)
                    dbus_message_unref(msg);
            });

            return msg;
        }

        std::shared_ptr<DBusMessage> message_new_set(const std::string &service_bus_name, const std::string &path,
                                                     const std::string &for_interface,
                                                     const std::string &property_name) {
            auto member = "Set";
            auto msg_p = dbus_message_new_method_call(service_bus_name.c_str(), path.c_str(), iface, member);
            if (msg_p) {
                DBusMessageIter iter{nullptr};
                dbus_message_iter_init_append(msg_p, &iter);
                auto p_for_iface = for_interface.c_str();
                dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &p_for_iface);
                auto p_prop_name = property_name.c_str();
                dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &p_prop_name);
            }

            std::shared_ptr<DBusMessage> msg(msg_p, [](DBusMessage *msg) {
                if (msg)
                    dbus_message_unref(msg);
            });

            return msg;
        }

        bool is_signal(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Signals signal) {
            std::string member;
            switch (signal) {
                case Signals::PropertiesChanged:
                    member = "PropertiesChanged";
                    break;

                default:
                    break;
            }

            return (msg_info.path == path &&
                    msg_info.iface == pie::dbus::properties::iface &&
                    msg_info.member == member);
        }

        std::shared_ptr<DBusMessage> message_new_signal(const std::string &path, Signals signal) {
            auto signal_as_string = to_string(signal);
            auto msg_p = dbus_message_new_signal(path.c_str(), iface, signal_as_string.c_str());
            std::shared_ptr<DBusMessage> msg(msg_p, [](DBusMessage *msg) {
                if (msg)
                    dbus_message_unref(msg);
            });

            return msg;
        }

        std::string to_string(Signals signal) {
            switch (signal) {
                case Signals::PropertiesChanged:
                    return "PropertiesChanged";
                default:
                    return "Unknown";
            }
        }

        Arguments get_arguments(DBusMessage *message) {
            Arguments arguments;
            DBusMessageIter args;
            if (dbus_message_iter_init(message, &args)) {
                if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&args)) {
                    char *value{nullptr};
                    dbus_message_iter_get_basic(&args, &value);
                    arguments.iface = value;
                }

                if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&args)) {
                    char *value{nullptr};
                    dbus_message_iter_get_basic(&args, &value);
                    arguments.property = value;
                }
            }

            return arguments;
        }
    }
}
