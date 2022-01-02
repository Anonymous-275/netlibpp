///
/// Created by Anonymous275 on 1/2/22
/// Copyright (c) 2021-present Anonymous275 read the LICENSE file for more info.
///

#pragma once
#include "netlibpp_enums.h"
#include <functional>
#include <vector>
#include <string>

namespace netlibpp {
    typedef std::function<void(const std::string&)> errorFunc;
    typedef std::vector<char> messageType;
    class Client {
    public:
        explicit Client(errorFunc ErrorHandler, const protocol& Protocol = TCP) noexcept;
        explicit Client(const protocol& Protocol = TCP) noexcept;
        ~Client();
    public:
        inline void set_error_function(const errorFunc& ErrorHandler) { ErrFn = ErrorHandler; };
        size_t send(const std::string& msg, flags flag = NONE);
        messageType receive(size_t length, flags flag = NONE);
        inline bool connected() const { return Connected; };
        inline bool has_error() const { return Err != 0; };
        void set_target(const char* Address, int Port);
        std::string get_error();
        void connect();
    private:
        inline void handleError(bool netc = false);
        inline void killSocket(NETC_SOCKET Dead);
        errorFunc ErrFn = nullptr;
        bool Connected = false;
        bool HasTarget = false;
        NETC_SOCKET SOCK = -1;
        std::string ErrBuf{};
        protocol Proto;
        int Err = 0;
        void init();
    };
}
