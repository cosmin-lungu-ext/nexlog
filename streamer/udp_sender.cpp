#include "streamer/udp_sender.hpp"
#include <iostream>
#include <cstring>

namespace nexlog {

UDPSender::~UDPSender() {
    close();
}

bool UDPSender::init(const std::string& host, uint16_t port) {
    host_ = host;
    port_ = port;

#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "UDPSender: WSAStartup failed" << std::endl;
        return false;
    }
    winsock_initialized_ = true;
#endif

    socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_ == INVALID_SOCKET) {
        std::cerr << "UDPSender: failed to create socket" << std::endl;
        return false;
    }

    connected_ = true;
    std::cout << "UDPSender: ready to stream to " << host << ":" << port << std::endl;
    return true;
}

bool UDPSender::send(const DiagPacket& packet) {
    if (!connected_) return false;

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port_);
    inet_pton(AF_INET, host_.c_str(), &server_addr.sin_addr);

    int result = sendto(socket_,
                        reinterpret_cast<const char*>(&packet),
                        sizeof(DiagPacket),
                        0,
                        reinterpret_cast<struct sockaddr*>(&server_addr),
                        sizeof(server_addr));

    return result != -1;
}

void UDPSender::close() {
    if (socket_ != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(socket_);
#else
        ::close(socket_);
#endif
        socket_ = INVALID_SOCKET;
    }
    connected_ = false;

#ifdef _WIN32
    if (winsock_initialized_) {
        WSACleanup();
        winsock_initialized_ = false;
    }
#endif
}

} // namespace nexlog