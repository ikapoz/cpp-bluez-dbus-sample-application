/**
* @file Logger.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include <string>

namespace pie {
    enum class LogLevel {
        Trace,
        Debug,
        Information,
        Warning,
        Error,
        None
    };

    class Logger {
    public:
        virtual ~Logger() = default;

        virtual void log(LogLevel level, std::string message) = 0;
    };
}
