///
/// Created by Anonymous275 on 1/2/22
/// Copyright (c) 2021-present Anonymous275 read the LICENSE file for more info.
///

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