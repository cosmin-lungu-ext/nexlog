#include <iostream>
#include "common/message.hpp"
#include "common/signal_decoder.hpp"

int main(int argc, char* argv[]) {
    std::cout << "nexlog starting..." << std::endl;
    std::cout << "------------------------" << std::endl;

    // Test 1 — Engine Oil Pressure
    nexlog::J1939Message pressure_msg;
    pressure_msg.timestamp_ms   = 1000;
    pressure_msg.pgn            = nexlog::PGN::ENGINE_FLUID_PRESSURE;
    pressure_msg.source_address = nexlog::SA::ENGINE;
    pressure_msg.length         = 8;
    pressure_msg.data           = {0xFF, 0xFF, 0xFF, 0x3C, 0xFF, 0xFF, 0xFF, 0xFF};

    std::cout << pressure_msg.to_string() << std::endl;
    auto decoded = nexlog::SignalDecoder::decode(pressure_msg);
    if (decoded.has_value()) {
        std::cout << decoded->to_string() << std::endl;
    }

    std::cout << "------------------------" << std::endl;

    // Test 2 — Vehicle Speed
    nexlog::J1939Message speed_msg;
    speed_msg.timestamp_ms   = 2000;
    speed_msg.pgn            = nexlog::PGN::VEHICLE_SPEED;
    speed_msg.source_address = nexlog::SA::ENGINE;
    speed_msg.length         = 8;
    speed_msg.data           = {0xF0, 0x2B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    std::cout << speed_msg.to_string() << std::endl;
    auto decoded2 = nexlog::SignalDecoder::decode(speed_msg);
    if (decoded2.has_value()) {
        std::cout << decoded2->to_string() << std::endl;
    }

    std::cout << "------------------------" << std::endl;

    // Test 3 — Unsupported PGN
    nexlog::J1939Message unknown_msg;
    unknown_msg.pgn    = 0x1234;
    unknown_msg.length = 8;

    auto decoded3 = nexlog::SignalDecoder::decode(unknown_msg);
    if (!decoded3.has_value()) {
        std::cout << "PGN 0x1234: not supported" << std::endl;
    }

    return 0;
}