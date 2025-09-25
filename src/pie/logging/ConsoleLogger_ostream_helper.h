/**
* @file ConsoleLogger_ostream_helper.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once
#include "ConsoleLogger.h"

#include <ostream>
#include <sstream>

inline std::ostream &operator<<(std::ostream &os, const pie::LogLevel &log_level) {
    switch (log_level) {
        case pie::LogLevel::Trace:
            os << "TRC";
            break;
        case pie::LogLevel::Debug:
            os << "DBG";
            break;
        case pie::LogLevel::Information:
            os << "INF";
            break;
        case pie::LogLevel::Warning:
            os << "WRN";
            break;
        case pie::LogLevel::Error:
            os << "ERR";
            break;
        default:
            os << "___";
            break;
    }

    return os;
}
