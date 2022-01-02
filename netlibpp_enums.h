///
/// Created by Anonymous275 on 1/2/22
/// Copyright (c) 2021-present Anonymous275 read the LICENSE file for more info.
///

#pragma once

#if defined(WIN32) || defined(__MINGW32__) || defined(_MSC_VER)
#define NETC_WINDOWS
#define NETC_SOCKET uintptr_t
#else
#define NETC_SOCKET int
#endif

namespace netlibpp {
    enum protocol {
        TCP = 1,
        UDP = 2
    };

    enum flags {
        //default no flags
        NONE = 0x0,
        //send or receive flag
        //Will make sure the full message will be sent or received until buffer saturation
        //Will apply MSG_WAITALL for the receive function
        NETC_ENFORCE_LENGTH = -1,
        //send or receive flag
        //Sends out-of-band data on sockets that support out-of-band communications.
        MSG_OOB = 0x01,
        //receive flag
        //Peeks at the incoming data. The data is copied into the buffer, but is not removed from the input queue.
        MSG_PEEK = 0x2,
#ifdef NETC_WINDOWS
        //receive flag
        //The receive request will complete only when one of the following events occurs:
        //The buffer supplied by the caller is completely full.
        //The connection has been closed.
        //The request has been canceled or an error occurred.
        MSG_WAITALL = 0x8,
        //send flag
        //Specifies that the data should not be subject to routing.
        //A Windows Sockets service provider can choose to ignore this flag.
        MSG_DONTROUTE = 0x4
#else
        //receive flag
        //Requests that the function block until the full amount of data requested can be returned.
        //The function may return a smaller amount of data if a signal is caught,
        //if the connection is terminated, if MSG_PEEK was specified, or if an error is pending for the socket.
        MSG_WAITALL = 0x100,
        //send flag
        //Terminates a record (if supported by the protocol)
        MSG_EOR	= 0x80,
#endif
    };
}