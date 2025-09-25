/**
* @file DBus.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "DBus.h"
#include "pie/dbus/helper/dbus.h"
#include "pie/dbus/DBusOnMessage.h"
#include "pie/concurrent/ConcurrentQueue.h"
#include "pie/logging/console_helpers.h"
#include "helper/DBusMessageExecuteBase.h"
#include "helper/SendDBusMessageExecute.h"
#include "helper/SendWithReplyDBusMessageExecute.h"

#include <thread>

namespace {
    uint32_t tag_cnt{0};
    uint32_t cnt{0};

    const std::string TAG{"DBus"};

    void log_msg(std::shared_ptr<pie::Logger> &logger, std::shared_ptr<DBusMessage> msg, const std::string &prefix) {
        auto msg_info = pie::dbus::get_message_info(msg);
        std::stringstream ss{};
        ss << prefix << " message: " << msg_info;
        pie::logger::log_if_debug(logger, pie::LogLevel::Trace, ss.str());
    }
}

namespace pie::dbus {
    struct DBusData {
        std::shared_ptr<pie::Logger> logger;
        std::thread dbus_thread;
        pie::dbus::DBusState state{pie::dbus::DBusState::Stopped};
        DBusConnection *conn{nullptr};
        std::vector<std::weak_ptr<DBusOnMessage> > subscribers{};
        std::string tag;
        pie::concurrent::ConcurrentQueue<std::shared_ptr<DBusMessageExecuteBase> > msg_queue{};
    };

    DBus::DBus(const std::shared_ptr<pie::Logger> &logger) {
        data = std::make_unique<DBusData>();
        std::stringstream ss{};
        ss << TAG << tag_cnt++;
        data->tag = ss.str();
        data->logger = logger;
        data->state = pie::dbus::DBusState::Initializing;
        data->dbus_thread = std::thread(&DBus::execute, this);

        auto i = 0;
        while (i < 10) {
            if (i < 5)
                std::this_thread::sleep_for(1ms);
            else
                std::this_thread::sleep_for(10ms);
            ++i;
            if (data->state == DBusState::Running)
                break;
        }

        if (data->state != DBusState::Running)
            data->state = DBusState::Error;
    }

    DBus::~DBus() {
        data->state = DBusState::Stopped;
        if (data->dbus_thread.joinable())
            data->dbus_thread.join();

        pie::logger::log_if_debug(data->logger, LogLevel::Trace, "DBus::~Dbus");
    }


    DBusState DBus::state() const {
        return data->state;
    }

    void DBus::subscribe(const std::weak_ptr<pie::dbus::DBusOnMessage> &subscriber) {
        data->subscribers.emplace_back(subscriber);
    }


    void DBus::execute() {
        DBusError dbus_error{};
        dbus_error_init(&dbus_error);
        auto logger = data->logger;
        auto conn = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
        if (dbus_error_is_set(&dbus_error)) {
            std::stringstream ss{};
            ss << "DBus error name: " << dbus_error.name;
            ss << ", message: " << dbus_error.message;
            pie::logger::log_if_debug(logger, data->tag, LogLevel::Trace, ss.str());
            dbus_error_free(&dbus_error);
            data->state = DBusState::Error;
            return;
        }

        data->conn = conn;
        data->state = DBusState::Running;
        pie::logger::log_if_debug(logger, data->tag, LogLevel::Trace, "execute loop started");
        while (data->state == DBusState::Running) {
            try {
                if (!data->msg_queue.empty()) {
                    auto dbus_msg_exec = data->msg_queue.pop();
                    dbus_msg_exec->exec(conn);
                }

                dbus_connection_read_write_dispatch(conn, 1);
                auto msg_p = dbus_connection_borrow_message(conn);
                if (msg_p) {
                    bool return_msg{true};
                    // parse message
                    std::shared_ptr<DBusMessage> msg{
                        msg_p, [](DBusMessage *msg) {
                        }
                    };
                    auto msg_info = pie::dbus::get_message_info(msg);
                    auto msg_info_as_string = pie::dbus::get_message_info(msg_info);
                    pie::logger::log_if_debug(logger, data->tag, LogLevel::Trace, msg_info_as_string);
                    for (const auto &weak_subscriber: data->subscribers) {
                        if (auto subscriber = weak_subscriber.lock()) {
                            auto result = subscriber->on_message(msg_info, msg);
                            if (result == DBUS_HANDLER_RESULT_HANDLED ||
                                result == DBUS_HANDLER_RESULT_NEED_MEMORY) {
                                dbus_connection_steal_borrowed_message(conn, msg_p);
                                dbus_message_unref(msg_p);
                                return_msg = false;
                                break;
                            }
                        }
                    }

                    if (return_msg)
                        dbus_connection_return_message(conn, msg_p);
                }
            } catch (const std::exception &e) {
                logger->log(LogLevel::Warning, e.what());
            }
        }


        pie::logger::log_if_debug(logger, data->tag, LogLevel::Trace, "execute loop ended");
    }


    std::tuple<DBusResult, std::shared_ptr<DBusMessage> > DBus::send_with_reply(
        std::shared_ptr<DBusMessage> &&msg, std::chrono::milliseconds max_wait_time) {
        std::stringstream ss{};
        ss << data->tag << "|DBus::send_with_reply";
        log_msg(data->logger, msg, ss.str());
        auto max_wait_time_ms = max_wait_time.count();
        auto cmd = std::make_shared<pie::dbus::SendWithReplyDBusMessageExecute>(std::move(msg));
        auto sleep = std::chrono::milliseconds(1);
        data->msg_queue.push(cmd);
        auto cnt{0};
        while (cnt < max_wait_time_ms) {
            std::this_thread::sleep_for(sleep);
            cnt++;
            if (cmd->status() == DBusMessageExecuteBase::Status::Finished)
                break;
        }

        if (cmd->status() == DBusMessageExecuteBase::Status::Finished) {
            auto result_op = cmd->result();
            if (result_op.has_value())
                return result_op.value();

            DBusResult dbus_result{};
            dbus_result.code = DBusResultCode::Error;
            dbus_result.error = "Internal Error";
            return {dbus_result, nullptr};
        }

        DBusResult dbus_result{};
        dbus_result.code = DBusResultCode::E_CMD_Timeout;
        dbus_result.error = "Command Timeout";
        return {dbus_result, nullptr};
    }

    DBusResult DBus::send(std::shared_ptr<DBusMessage> &&msg, std::chrono::milliseconds max_wait_time) {
        std::stringstream ss{};
        ss << data->tag << "|DBus::send";
        auto max_wait_time_ms = max_wait_time.count();
        ss << " max_wait_time_ms: " << max_wait_time_ms << ", ";
        log_msg(data->logger, msg, ss.str());
        auto cmd = std::make_shared<pie::dbus::SendDBusMessageExecute>(std::move(msg));
        auto sleep = std::chrono::milliseconds(1);
        data->msg_queue.push(cmd);
        auto cnt{0};
        while (cnt < max_wait_time_ms) {
            std::this_thread::sleep_for(sleep);
            cnt++;
            if (cmd->status() == DBusMessageExecuteBase::Status::Finished)
                break;
        }

        if (cnt >= max_wait_time_ms) {
            DBusResult dbus_result{};
            dbus_result.code = DBusResultCode::E_CMD_Timeout;
            dbus_result.error = "Command Timeout";
            return dbus_result;
        }

        if (cmd->status() == DBusMessageExecuteBase::Status::Finished) {
            auto result_op = cmd->result();
            if (result_op.has_value()) {
                auto [result, msg_rsp] = result_op.value();
                return result;
            }
        }

        DBusResult dbus_result{};
        dbus_result.code = DBusResultCode::Error;
        dbus_result.error = "Internal Error";
        return dbus_result;
    }

    DBusResult DBus::reply(std::shared_ptr<DBusMessage> &&msg) {
        std::stringstream ss{};
        ss << data->tag << "|DBus::reply";
        log_msg(data->logger, msg, ss.str());
        auto current_id = std::this_thread::get_id();
        auto dbus_thread_id = data->dbus_thread.get_id();
        if (dbus_thread_id != current_id) {
            DBusResult dbus_result{};
            dbus_result.code = DBusResultCode::E_CMD_Timeout;
            dbus_result.error = "Can be called only on DBus thread";
            return dbus_result;
        }

        uint32_t id{0};
        auto success = dbus_connection_send(data->conn, msg.get(), &id);
        dbus_connection_flush(data->conn);

        auto dbus_result = pie::dbus::DBusResult{};
        if (!success) {
            dbus_result.code = DBusResultCode::Error;
            dbus_result.error = "Failed to send message";
        }

        return dbus_result;
    }


    std::shared_ptr<DBusMessage> DBus::new_message(std::string &bus_name, std::string &path, std::string &iface,
                                                   std::string &method) {
        auto msg_p = dbus_message_new_method_call(bus_name.c_str(), path.c_str(), iface.c_str(), method.c_str());
        std::shared_ptr<DBusMessage> msg{
            msg_p, [](DBusMessage *msg) {
                if (msg)
                    dbus_message_unref(msg);
            }
        };
        return msg;
    }
} // pie
