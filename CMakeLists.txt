cmake_minimum_required(VERSION 3.21)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)

project(cpp_bluez_dbus_tx_example
        DESCRIPTION "C++ BlueZ DBus TX example"
        VERSION 0.1.0
        LANGUAGES CXX
)

add_executable(${PROJECT_NAME})

find_package(PkgConfig)
pkg_check_modules(PIE_LIBS REQUIRED dbus-1)
target_include_directories(${PROJECT_NAME} PRIVATE ${PIE_LIBS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PIE_LIBS_LIBRARIES})

target_sources(${PROJECT_NAME}
        PRIVATE
        src/pie/bluez/gatt/helper/characteristic.h
        src/pie/bluez/gatt/helper/manager.h
        src/pie/bluez/gatt/helper/service.h
        src/pie/bluez/gatt/Characteristic.h
        src/pie/bluez/gatt/Exception.h
        src/pie/bluez/gatt/Server.h
        src/pie/bluez/gatt/Service.h
        src/pie/bluez/helper/bluez.h
        src/pie/bluez/helper/le_advertisement.h
        src/pie/bluez/helper/le_advertising_manager.h
        src/pie/bluez/GattManager.h
        src/pie/bluez/HostControllerInterface.h
        src/pie/bluez/LEAdvertisement.h
        src/pie/bluez/LEAdvertisingManager.h
        src/pie/concurrent/ConcurrentQueue.h
        src/pie/dbus/helper/dbus.h
        src/pie/dbus/helper/DBusMessageExecuteBase.h
        src/pie/dbus/helper/SendDBusMessageExecute.h
        src/pie/dbus/helper/SendWithReplyDBusMessageExecute.h
        src/pie/dbus/DBus.h
        src/pie/dbus/DBusException.h
        src/pie/dbus/DBusObjectManager.h
        src/pie/dbus/DBusOnMessage.h
        src/pie/logging/console_helpers.h
        src/pie/logging/ConsoleLogger.h
        src/pie/logging/ConsoleLogger_ostream_helper.h
        src/pie/logging/Logger.h
        src/pie/GattSampleServer.h

        src/pie/bluez/gatt/helper/characteristic.cpp
        src/pie/bluez/gatt/helper/manager.cpp
        src/pie/bluez/gatt/helper/service.cpp
        src/pie/bluez/gatt/Characteristic.cpp
        src/pie/bluez/gatt/Exception.cpp
        src/pie/bluez/gatt/Service.cpp
        src/pie/bluez/helper/le_advertisement.cpp
        src/pie/bluez/helper/le_advertising_manager.cpp
        src/pie/bluez/HostControllerInterface.cpp
        src/pie/bluez/GattManager.cpp
        src/pie/bluez/LEAdvertisement.cpp
        src/pie/bluez/LEAdvertisingManager.cpp
        src/pie/dbus/helper/dbus.cpp
        src/pie/dbus/helper/DBusMessageExecuteBase.cpp
        src/pie/dbus/helper/SendDBusMessageExecute.cpp
        src/pie/dbus/helper/SendWithReplyDBusMessageExecute.cpp
        src/pie/dbus/DBus.cpp
        src/pie/dbus/DBusException.cpp
        src/pie/dbus/DBusOnMessage.cpp
        src/pie/logging/ConsoleLogger.cpp
        src/pie/GattSampleServer.cpp
        src/main.cpp
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)