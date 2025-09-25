/**
* @file main.cpp
* @author Ilija Poznic
* @date 2025
*/

#include "pie/dbus/DBus.h"
#include "pie/GattSampleServer.h"
#include "pie/bluez/LEAdvertisement.h"
#include "pie/bluez/LEAdvertisingManager.h"
#include <pie/logging/ConsoleLogger.h>


#include <cstdlib>
#include <exception>
#include <iostream>

int main(int argc, char **argv, char **envp) {
    try {
        auto console_logger = std::make_shared<pie::logging::ConsoleLogger>();
        auto dbus = std::make_shared<pie::dbus::DBus>(console_logger);
        auto gatt_sample_server = std::make_shared<pie::GattSampleServer>(dbus, console_logger);
        gatt_sample_server->start();
        std::string exit;
        std::cout << "Press ENTER to exit server" << std::endl;
        std::getline(std::cin, exit);
        std::cout << "Exiting" << std::endl;
        gatt_sample_server->stop();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
