/**
* @file OnValueChanged.h
* @author Ilija Poznic
* @date 2025
*/

#pragma once

#include <vector>
#include <string>

namespace pie::bluez::gatt {
    class OnValueChanged {
    public:
        virtual ~OnValueChanged() = default;

        /**
         * @param uuid GATT
         * @param value new value received
         * @return value that will be sent back and cached
         */
        virtual void on_value_changed(const std::string &uuid,
                                      const std::vector<uint8_t> &value) = 0;
    };
}
