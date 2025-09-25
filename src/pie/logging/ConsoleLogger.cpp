/**
* @file ConsoleLogger.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "ConsoleLogger.h"
#include "ConsoleLogger_ostream_helper.h"
#include <iostream>

namespace pie::logging {

    void ConsoleLogger::log_level(const LogLevel set) {
        level = set;
    }

    void ConsoleLogger::log(LogLevel as_level, std::string message) {
        if (as_level >= this->level) {
            std::cout << level << " | " << message << std::endl;
            if (level == LogLevel::Error)
                std::cerr << level << " | " << message << std::endl;
        }
    }
}
