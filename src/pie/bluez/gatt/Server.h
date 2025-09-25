/**
* @file Server.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

namespace pie::bluez::gatt {
    enum class ServerState {
        Error,
        Running,
        Stopped
    };

    class Server {
    public:
        virtual ~Server() = default;

        virtual void start() = 0;

        virtual void stop() = 0;

        [[nodiscard]] virtual ServerState state() const = 0;
    };
}
