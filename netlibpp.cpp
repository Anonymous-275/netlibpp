///
/// Created by Anonymous275 on 1/2/22
/// Copyright (c) 2021-present Anonymous275 read the LICENSE file for more info.
///

#include "netlibpp.h"

#ifdef NETC_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define NETC_RET_TYPE int
#define NETC_LEN_TYPE int
#else
#include <arpa/inet.h>
#include <cerrno>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <utility>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SD_BOTH SHUT_RDWR
#define NETC_RET_TYPE ssize_t
#define NETC_LEN_TYPE uint32_t
#endif

namespace netlibpp {
    sockaddr_in netc_sockaddr{};

    Client::Client(const protocol& Protocol) noexcept :
    Proto(Protocol) {
        init();
    }


    Client::Client(errorFunc ErrorHandler, const protocol& Protocol) noexcept :
    ErrFn(std::move(ErrorHandler)), Proto(Protocol) {
        init();
    }


    inline void Client::handleError(bool netc) {
        if(!netc && Err == 0) {
#ifdef NETC_WINDOWS
            Err = WSAGetLastError();
#else
            Err = errno;
#endif
        }
        if(ErrFn) {
            ErrFn(get_error());
            Err = 0;
        }
    }

    void Client::init() {
#ifdef NETC_WINDOWS
        WSADATA wsaData;
        int Res = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (Res != 0) {
            Err = Res;
            handleError(false);
        }
#endif
        switch (Proto) {
            case TCP:
                SOCK = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                break;
            case UDP:
                SOCK = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                break;
        }
        if(SOCK == INVALID_SOCKET) {
            handleError(false);
        }
    }


    std::string Client::get_error() {
        std::string Ret(256, 0);
        if(!ErrBuf.empty()){
            Ret = ErrBuf;
            ErrBuf = "";
            return Ret;
        }
#ifdef NETC_WINDOWS
        auto Res = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                  nullptr,
                                  Err,
                                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                  &Ret[0],
                                  DWORD(Ret.size()),
                                  nullptr);
        if(Res != 0)Ret.resize(Res);
#else
        Ret = std::strerror(Err);
#endif
        return Ret;
    }

    inline void Client::killSocket(NETC_SOCKET Dead) {
        if(Dead == INVALID_SOCKET)return;
        shutdown(Dead, SD_BOTH);
#ifdef NETC_WINDOWS
        if(closesocket(Dead) != 0 && WSAGetLastError() != 10038) {
            handleError();
        }
#else
        if(close(Dead) != 0 && errno != EBADF){
            handleError();
        }
#endif
        Connected = false;
    }

    void Client::set_target(const char* Address, int Port) {
        netc_sockaddr.sin_family = AF_INET;
        netc_sockaddr.sin_port = htons(Port);
        netc_sockaddr.sin_addr.s_addr = inet_addr(Address);
        HasTarget = true;
    }

    void Client::connect() {
        if(Connected) {
            ErrBuf = "Already connected to a server";
            handleError(true);
            return;
        } else if(Proto != TCP) {
            ErrBuf = "Cannot connect! Protocol is not TCP";
            handleError(true);
            return;
        } else if(!HasTarget) {
            ErrBuf = "Cannot connect! Please call the set_target method first";
            handleError(true);
            return;
        }
        int Res = ::connect(SOCK, (sockaddr*)&netc_sockaddr, sizeof(netc_sockaddr));
        if(Res != 0){
            handleError();
            killSocket(SOCK);
            return;
        }
        Connected = true;
    }


    size_t Client::send(const std::string& msg, flags flag) {
        if(flag == NETC_ENFORCE_LENGTH && Proto != TCP)flag = NONE;
        NETC_LEN_TYPE len = sizeof(netc_sockaddr);
        NETC_RET_TYPE res;
        size_t total = 0;
        auto final_flag = flag == NETC_ENFORCE_LENGTH ? 0 : flag;
        do {
            res = ::sendto(SOCK, &msg[total], int(msg.size() - total), final_flag, (sockaddr*)&netc_sockaddr, len);
            total += res;
        }while(flag == NETC_ENFORCE_LENGTH && res != SOCKET_ERROR && total < msg.size());
        if (res == SOCKET_ERROR) {
            handleError();
            killSocket(SOCK);
            return 0;
        }
        return total;
    }

    messageType Client::receive(size_t length, flags flag) {
        if(flag == NETC_ENFORCE_LENGTH && Proto != TCP)flag = NONE;
        NETC_LEN_TYPE len = sizeof(netc_sockaddr);
        NETC_RET_TYPE res;
        size_t total = 0;
        messageType buffer(length + 1, 0);
        auto final_flag = flag == NETC_ENFORCE_LENGTH ? MSG_WAITALL : flag;
        do {
            res = ::recvfrom(SOCK, &buffer[total], int(length - total), final_flag, (sockaddr*)&netc_sockaddr, &len);
            total += res;
        } while(flag == NETC_ENFORCE_LENGTH && res > 0 && total < length);
        if (res == SOCKET_ERROR) {
            handleError();
            killSocket(SOCK);
            return {};
        } else if(res == 0) {
            Connected = false;
            killSocket(SOCK);
        }
        buffer.resize(total);
        return buffer;
    }

    Client::~Client() {
        Connected = false;
        killSocket(SOCK);
#ifdef NETC_WINDOWS
        WSACleanup();
#endif
    }
}