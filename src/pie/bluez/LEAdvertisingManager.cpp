/**
* @file LEAdvertisingManager.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "LEAdvertisingManager.h"

#include "pie/logging/console_helpers.h"
#include "helper/bluez.h"
#include "helper/le_advertising_manager.h"

#include <utility>
#include <vector>

namespace {
    const inline std::string TAG = "LEAdvertisingManager";
}

namespace pie::bluez {
    struct LEAdvertisingManagerData {
        std::string service{pie::bluez::service_org_bluez};
        std::string iface{le_advertising_manager::iface};
        std::string path;
        std::shared_ptr<Logger> logger{nullptr};
        std::shared_ptr<dbus::DBus> dbus{nullptr};
        std::shared_ptr<bluez::LEAdvertisingManager> self{nullptr};
        std::vector<std::shared_ptr<LEAdvertisement> > advertisements{};
    };

    LEAdvertisingManager::LEAdvertisingManager(
        std::string path, std::shared_ptr<dbus::DBus> dbus, std::shared_ptr<Logger> logger) {
        data = std::make_unique<LEAdvertisingManagerData>();
        data->path = std::move(path);
        data->logger = std::move(logger);
        data->dbus = std::move(dbus);
        std::shared_ptr<LEAdvertisingManager> self(this, [](LEAdvertisingManager *) {
        });
        data->dbus->subscribe(self);
        data->self = std::move(self);
    }

    LEAdvertisingManager::~LEAdvertisingManager() {
        for (const auto &item: data->advertisements)
            unregister_advertisement(item);

        data->advertisements.clear();
        data->self.reset();
        pie::logger::log_if_debug(data->logger, LogLevel::Trace, "LEAdvertisingManager::~LEAdvertisingManager()");
    }

    bool LEAdvertisingManager::register_advertisement(const std::shared_ptr<LEAdvertisement> &advertisement) {
        auto method = std::string("RegisterAdvertisement");
        auto msg = pie::dbus::DBus::new_message(data->service,
                                                data->path, data->iface,
                                                method);
        data->advertisements.emplace_back(advertisement);
        advertisement->register_advertisement(msg);
        auto is_success = true;
        auto result =  data->dbus->send(std::move(msg));
        if (result.code != dbus::DBusResultCode::Success) {
            std::stringstream ss;
            ss << "Failed RegisterAdvertisement: " << result.error;
            logger::log(data->logger, TAG, LogLevel::Warning, ss.str());
            is_success = false;
            // TODO remove from data->advertisements
        }

        // auto [result, msg_reply] =  data->dbus->send_with_reply(std::move(msg), 2000ms);
        // if (result.code != dbus::DBusResultCode::Success) {
        //     std::stringstream ss;
        //     ss << "Failed RegisterAdvertisement: " << result.error;
        //     logger::log(data->logger, TAG, LogLevel::Warning, ss.str());
        //     is_success = false;
        //     // TODO remove from data->advertisements
        // }
        return is_success;
    }

    void LEAdvertisingManager::unregister_advertisement(const std::shared_ptr<LEAdvertisement> &advertisement) {
        auto method = std::string("UnregisterAdvertisement");
        auto msg = pie::dbus::DBus::new_message(data->service,
                                                data->path, data->iface,
                                                method);

        advertisement->unregister_advertisement(msg);
        auto [result, msg_reply] = data->dbus->send_with_reply(std::move(msg));
        if (result.code != dbus::DBusResultCode::Success) {
            std::stringstream ss;
            ss << "Failed UnregisterAdvertisement error: " << result.error;
            pie::logger::log(data->logger, TAG, LogLevel::Warning, ss.str());
        }

        // TODO remove advertisement from data->advertisements
        //        auto remove = std::remove(data->advertisements.begin(), data->advertisements.end(),advertisement)
    }

    DBusHandlerResult LEAdvertisingManager::on_message(const dbus::DBusMessageInfo &msg_info,
                                                       std::shared_ptr<DBusMessage> message) {
        for (const auto &item: data->advertisements) {
            auto result = item->on_message(msg_info, message);
            if (result == DBUS_HANDLER_RESULT_NEED_MEMORY || result == DBUS_HANDLER_RESULT_HANDLED)
                return result;
        }

        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
} // pie
