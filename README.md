# netlibpp
[![CMake Linux Build](https://github.com/Anonymous-275/netlibpp/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/Anonymous-275/netlibpp/actions/workflows/cmake-linux.yml)
[![CMake Windows Build](https://github.com/Anonymous-275/netlibpp/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/Anonymous-275/netlibpp/actions/workflows/cmake-windows.yml)

netlibpp is a modern cross-platform C++ network library for developing network services using TCP/UDP protocols,
it supports C++11, C++14, C++17, and C++20.

TCP client example:
```cpp
#include "../netlibpp.h"
#include <iostream>
int main() {
    netlibpp::Client Client([](const std::string& error) {
        std::cout << "Error: " << error;
    }, netlibpp::TCP);

    if(Client.has_error())return 1;

    Client.set_target("127.0.0.1", 2030);
    Client.connect();

    while(Client.connected()) {
        Client.send("hello\n");
        auto message = Client.receive(8);
        if(!message.empty()) {
            std::cout << message.data() << " : " << message.size() << "\n";
        }
    }

    return 0;
}
```

UDP client example:
```cpp
#include "../netlibpp.h"
#include <iostream>

int main() {
    netlibpp::Client Client([](const std::string& error) {
        std::cout << "Error: " << error;
    }, netlibpp::UDP);

    if(Client.has_error())return 1;

    Client.set_target("127.0.0.1", 2031);
    netlibpp::messageType message;

    do {
        Client.send("hello\n");
        message = Client.receive(8);
        if (!message.empty()) {
            std::cout << message.data() << " : " << message.size() << "\n";
        }
    } while(!Client.has_error() && !message.empty());

    return 0;
}
```
