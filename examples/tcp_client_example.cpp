///
/// Created by Anonymous275 on 1/2/22
/// Copyright (c) 2021-present Anonymous275 read the LICENSE file for more info.
///

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