/**
* @file console_helpers.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once


#include "pie/logging/Logger.h"

#include <memory>
#include <string>
#include <sstream>

namespace pie::logger {
    inline void log_if_debug(const std::shared_ptr<pie::Logger> &l, const LogLevel level, const std::string &message) {
#ifndef NDEBUG
        l->log(level, message);
#endif
    }

    inline void log(const std::shared_ptr<pie::Logger> &l, const std::string &tag, const LogLevel level,
                    const std::string &message) {
        std::stringstream ss;
        ss << tag << "|" << message;
        l->log(level, ss.str());
    }


    inline void log_if_debug(const std::shared_ptr<pie::Logger> &l, const std::string &tag, const LogLevel level,
                             const std::string &message) {
#ifndef NDEBUG
        log(l, tag, level, message);
#endif
    }
}
