#pragma once

#include "common/protocol.hpp"
#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    using SOCKET = int;
    constexpr int INVALID_SOCKET = -1;
#endif

namespace nexlog {

class UDPSender {
public:
    UDPSender() = default;
    ~UDPSender();

    // Initialize and connect to server
    bool init(const std::string& host, uint16_t port);

    // Send a diagnostic packet
    bool send(const DiagPacket& packet);

    // Close socket
    void close();

    bool is_connected() const { return connected_; }

private:
    SOCKET      socket_     = INVALID_SOCKET;
    bool        connected_  = false;
    std::string host_;
    uint16_t    port_       = 0;

#ifdef _WIN32
    bool winsock_initialized_ = false;
#endif
};

} // namespace nexlog