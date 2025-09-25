/**
* @file dbus.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once


#include "pie/dbus/DBus.h"
#include "pie/dbus/DBusOnMessage.h"

#include <pie/logging/Logger.h>
#include <dbus/dbus.h>

#include <string>
#include <memory>
#include <vector>

namespace pie::dbus {
    struct DBusResult;

    inline bool is_success(dbus_bool_t success) {
        return success == TRUE;
    }

    pie::dbus::DBusResult parse(DBusError *error);

    // pie::dbus::DBusMessageResult connection_send_with_reply_and_block(DBusConnection *connection,
    //                                                                   DBusMessage *msg,
    //
    //
    //                                                                   std::chrono::milliseconds timeout);

    pie::dbus::DBusMessageInfo get_message_info(std::shared_ptr<DBusMessage> msg);

    std::string get_message_info(const pie::dbus::DBusMessageInfo &msg_info);

    void throw_dbus_exception_if_error(const DBusResult &result);

    std::string message_type_as_string(DBusMessageType dbus_message_type);

    DBusMessageType message_get_type(int dbus_message_type);

    /**
     * Create new return method message
     * @param logger to log any warnings
     * @param message from whom will be created new return method message
     * @return bool - true if success and pointer to the reply message
     */
    std::tuple<bool, std::shared_ptr<DBusMessage> > message_new_method_return(
        const std::shared_ptr<pie::Logger> &logger,
        const std::shared_ptr<DBusMessage> &message);

    void message_append_dict_entry(DBusMessageIter *iter, const std::string &property_name, const std::string &value);

    void message_append_dict_entry(DBusMessageIter *iter, const std::string &property_name, bool value);

    void message_append_dict_entry(DBusMessageIter *iter, const std::string &property_name,
                                   const std::vector<std::string> &values);

    void message_append_dict_entry(DBusMessageIter *iter, const std::string &property_name,
                                   const std::vector<uint8_t> &values);

    void message_append_dict_entry_objects(DBusMessageIter *iter,
                                           const std::string &property_name,
                                           const std::vector<std::string> &objects);

    void message_append_dict_entry_object(DBusMessageIter *iter,
                                          const std::string &property_name,
                                          const std::string &object);

    void message_set_variant(DBusMessageIter *iter, const std::string &value);

    void message_set_variant(DBusMessageIter *iter, bool value);

    std::string message_get_variant_as_string(DBusMessageIter *iter);

    bool message_get_variant_as_boolean(DBusMessageIter *iter);

    std::string message_get_variant_as_object(DBusMessageIter *iter);

    uint16_t message_get_variant_as_uint16(DBusMessageIter *iter);

    std::vector<uint8_t> message_get_bytes(DBusMessageIter *iter);

    void message_variant_iter_next(DBusMessageIter *iter);

    bool is_match(const pie::dbus::DBusMessageInfo &msg_info,
                  pie::dbus::DBusMessageType type, const std::string &path,
                  const std::string &iface, const std::string &member);

    namespace object_manager {
        inline const char *iface = "org.freedesktop.DBus.ObjectManager";

        bool is_interface(const pie::dbus::DBusMessageInfo &msg_info);

        enum class Methods {
            GetManagedObject
        };

        enum class Signals {
            InterfacesAdded,
            InterfacesRemoved,
        };

        bool is_method(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Methods method);

        bool is_signal(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Signals signal);
    }


    namespace properties {
        inline const char *iface = "org.freedesktop.DBus.Properties";

        bool is_interface(const pie::dbus::DBusMessageInfo &msg_info);

        enum class Methods {
            Get,
            Set,
            GetAll
        };

        bool is_method(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Methods method);


        std::shared_ptr<DBusMessage> message_new_get_all(const std::string &service_bus_name,
                                                         const std::string &path,
                                                         const std::string &for_interface);

        std::shared_ptr<DBusMessage> message_new_get(const std::string &service_bus_name,
                                                     const std::string &path,
                                                     const std::string &for_interface,
                                                     const std::string &property_name);

        std::shared_ptr<DBusMessage> message_new_set(const std::string &service_bus_name,
                                                     const std::string &path,
                                                     const std::string &for_interface,
                                                     const std::string &property_name);

        // bool get(DBusConnection *connection,
        //               const std::string &service, const std::string &path,
        //               const std::string &property_name);
        //
        // bool
        //
        // bool send_

        enum class Signals {
            PropertiesChanged
        };

        bool is_signal(const pie::dbus::DBusMessageInfo &msg_info, const std::string &path, Signals signal);

        /**
         *
         * @param path - object path
         * @param signal - type
         * @return valid pointer to message. If nullptr than not enough memory to create message
         */
        std::shared_ptr<DBusMessage> message_new_signal(const std::string &path, Signals signal);

        std::string to_string(Signals signal);

        struct Arguments {
            std::string iface;
            std::string property;
        };

        Arguments get_arguments(DBusMessage *message);
    }
}
