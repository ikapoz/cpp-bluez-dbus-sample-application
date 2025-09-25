# C++ BlueZ DBus sample application

This project is a minimal-dependency C++ application that demonstrates how to 
create a Bluetooth Low Energy (BLE) server with advertising using the BlueZ stack 
via the D-Bus interface. 
It is designed to run on Linux systems and relies solely on the native D-Bus 
library—no external Bluetooth libraries required.

## Prerequisites?

### System Requirements

- Linux system with BlueZ installed and running
- Bluetooth hardware adapter (USB, PCIe, or built-in)
- CMake build system
- C++17 compatible compiler (e.g. GCC or CLang)

### Building

```shell
# Ubuntu/Debian
# install 
sudo apt install libdbus-1-dev cmake build-essential

# build
mkdir build && cd build
cmake ..
make
```

## Reference

- [BlueZ](https://github.com/bluez/bluez)
- [DBuz](https://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-objectmanager)