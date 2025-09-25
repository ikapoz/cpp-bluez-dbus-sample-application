/**
* @file ConsoleLogger.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once
#include "Logger.h"

namespace pie::logging {
    class ConsoleLogger : public Logger {
    public:
        void log_level(const LogLevel set);

        void log(LogLevel level, std::string message) override;

    private:
        LogLevel level{LogLevel::Trace};
    };
}
