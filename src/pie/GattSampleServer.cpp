/**
* @file GattSampleServer.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "GattSampleServer.h"
#include "bluez/gatt/Service.h"
#include "bluez/gatt/Characteristic.h"
#include "bluez/gatt/Exception.h"
#include "bluez/HostControllerInterface.h"
#include "bluez/LEAdvertisement.h"
#include "logging/console_helpers.h"


namespace pie {
    struct GattSampleServerData {
        std::string path;
        std::shared_ptr<dbus::DBus> dbus;
        std::shared_ptr<Logger> logger;
        std::shared_ptr<pie::bluez::HostControllerInterface> hci;
        std::shared_ptr<pie::GattSampleServer> self;
        std::shared_ptr<bluez::gatt::Service> service;
        std::shared_ptr<bluez::gatt::Characteristic> rx_chr;
        std::shared_ptr<bluez::LEAdvertisement> advertisement;
        bluez::gatt::ServerState state{bluez::gatt::ServerState::Stopped};
    };
}

namespace {
    inline const std::string TAG{"GattSampleServer"};
    inline const char *if_rs_pie = "rs.pie";
    inline const char *path_rs_pie = "/rs/pie";
    inline const char *path_rs_pie_gatt_sample_server = "/rs/pie/gatt_sample_server";

    DBusHandlerResult on_message_obj_mng_get_managed_object(
        const pie::dbus::DBusMessageInfo &msg_info,
        const std::shared_ptr<DBusMessage> &message,
        const std::shared_ptr<pie::GattSampleServerData> &data) {
        if (pie::dbus::object_manager::is_method(msg_info,
                                                 data->path, pie::dbus::object_manager::Methods::GetManagedObject)) {
            //  method: ObjectManager_GetManagedObject
            {
                std::stringstream ss{};
                ss << "on_message: path: " << msg_info.path;;
                ss << ", method: ObjectManager_GetManagedObject";
                pie::logger::log_if_debug(data->logger, TAG, pie::LogLevel::Trace,
                                          ss.str());
            }

            auto [success, reply_msg] = pie::dbus::message_new_method_return(data->logger, message);
            if (!success)
                return DBUS_HANDLER_RESULT_NEED_MEMORY;

            DBusMessageIter iter{nullptr};
            dbus_message_iter_init_append(reply_msg.get(), &iter);
            DBusMessageIter dict_iter{nullptr};
            // {oa{sa{sv}}}
            dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
                                             "{oa{sa{sv}}}", &dict_iter);
            // get managed objects (No Need as GattSampleServer has no implemented interface)
            //DBusMessageIter srv_iter{nullptr};

            // adding service(s)
            data->service->get_managed_objects(&dict_iter);

            dbus_message_iter_close_container(&iter, &dict_iter);
            dbus_message_iter_init_closed(&iter);

            auto result = data->dbus->reply(std::move(reply_msg));
            if (result.code != pie::dbus::DBusResultCode::Success) {
                std::stringstream ss{};
                ss << "response on_message: path: " << msg_info.path;;
                ss << ", method: ObjectManager_GetManagedObject";
                ss << ", error: " << result.error;
                data->logger->log(pie::LogLevel::Warning, ss.str());
                return DBUS_HANDLER_RESULT_NEED_MEMORY;
            }

            return DBUS_HANDLER_RESULT_HANDLED;
        }

        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
}


namespace pie {
    GattSampleServer::GattSampleServer(std::shared_ptr<pie::dbus::DBus> const &dbus,
                                       std::shared_ptr<Logger> const &logger) {
        data = std::make_shared<pie::GattSampleServerData>();
        data->dbus = dbus;
        data->logger = logger;
        data->path = path_rs_pie_gatt_sample_server;
        std::shared_ptr<pie::GattSampleServer> self(this, [](pie::GattSampleServer *server) {
        });
        data->self = self;
        data->dbus->subscribe(self);;
        data->hci = std::make_shared<pie::bluez::HostControllerInterface>(
            "/org/bluez/hci0", dbus, logger);

        // create service
        data->service = std::make_shared<bluez::gatt::Service>(
            service_uuid,
            data->path,
            true,
            data->dbus,
            data->logger);

        // create rx_characteristic
        std::vector<pie::bluez::gatt::characteristic::Flag> flags{
            bluez::gatt::characteristic::Flag::WriteWithoutResponse
        };

        data->rx_chr = std::make_shared<pie::bluez::gatt::Characteristic>(
            rx_uuid,
            data->service,
            std::vector{
                bluez::gatt::characteristic::Flag::WriteWithoutResponse
            },
            self,
            dbus,
            logger);

        data->service->add_characteristic(data->rx_chr);

        std::stringstream ss{};
        ss << data->path << "/le_advertisement";
        data->advertisement = std::make_shared<bluez::LEAdvertisement>(ss.str(), dbus, logger);

        data->advertisement->service_uuids({
            service_uuid,
        });

        data->advertisement->name(TAG);
    }

    GattSampleServer::~GattSampleServer() {
        GattSampleServer::stop();
        pie::logger::log_if_debug(data->logger,
                                  TAG, LogLevel::Trace,
                                  "GattSampleServer::~GattSampleServer()");
        data->self.reset();
    }

    void GattSampleServer::start() {
        if (data->state == pie::bluez::gatt::ServerState::Running)
            return;

        try {
            data->hci->gatt_manager()->register_application(data->path);
            data->hci->le_advertising_manager()->register_advertisement(data->advertisement);
            data->state = pie::bluez::gatt::ServerState::Running;
        } catch (const pie::bluez::gatt::Exception &ex) {
            std::stringstream ss{};
            ss << "Failed to start " << TAG;
            ss << ", error: " << ex.what();
            data->logger->log(LogLevel::Warning, ss.str());
            data->state = pie::bluez::gatt::ServerState::Error;
        } catch (...) {
            std::stringstream ss{};
            ss << "Failed to start " << TAG;
            data->logger->log(LogLevel::Warning, ss.str());
            data->state = pie::bluez::gatt::ServerState::Error;
        }
    }

    void GattSampleServer::stop() {
        if (data->state != pie::bluez::gatt::ServerState::Running)
            return;

        try {
            data->hci->gatt_manager()->unregister_application(data->path);
            data->hci->le_advertising_manager()->unregister_advertisement(data->advertisement);
            data->state = pie::bluez::gatt::ServerState::Stopped;
        } catch (const pie::bluez::gatt::Exception &ex) {
            std::stringstream ss{};
            ss << "Failed to stop " << TAG;
            ss << ", error: " << ex.what();
            data->logger->log(LogLevel::Warning, ss.str());
        } catch (...) {
            std::stringstream ss{};
            ss << "Failed to stop " << TAG;
            data->logger->log(LogLevel::Warning, ss.str());
        }
    }

    bluez::gatt::ServerState GattSampleServer::state() const {
        return data->state;
    }

    void GattSampleServer::on_value_changed(const std::string &uuid, const std::vector<uint8_t> &value) {
        std::stringstream ss{};
        ss << "Value changed for characteristic: " << uuid;
        ss << ", value: ";
        for (auto &byte: value) {
            auto ch = static_cast<char>(byte);
            if (isalnum(ch))
                ss << ch;
            else
                ss << ".";
        }

        data->logger->log(LogLevel::Information, ss.str());
    }


    DBusHandlerResult GattSampleServer::on_message(const dbus::DBusMessageInfo &msg_info,
                                                   std::shared_ptr<DBusMessage> message) {
        try {
            auto result = data->rx_chr->on_message(msg_info, message);
            if (result != DBUS_HANDLER_RESULT_NOT_YET_HANDLED)
                return result;

            result = data->advertisement->on_message(msg_info, message);
            if (result != DBUS_HANDLER_RESULT_NOT_YET_HANDLED)
                return result;

            if (msg_info.path == data->path) {
                result = on_message_obj_mng_get_managed_object(msg_info, message, data);
                if (result != DBUS_HANDLER_RESULT_NOT_YET_HANDLED)
                    return result;
            }
        } catch (...) {
        }

        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
} // pie
