#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <sstream>
#include <iomanip>

namespace nexlog {

// J1939 Parameter Group Numbers — common heavy equipment signals
namespace PGN {
    constexpr uint32_t ENGINE_FLUID_PRESSURE   = 0xFEEF;  // oil pressure, coolant
    constexpr uint32_t VEHICLE_SPEED           = 0xFEF1;  // wheel speed
    constexpr uint32_t ACTIVE_FAULT_CODES      = 0xFECA;  // DM1 diagnostics
    constexpr uint32_t ENGINE_HOURS            = 0xFEE5;  // total engine hours
    constexpr uint32_t ENGINE_TEMP             = 0xFEEE;  // coolant temperature
}

// J1939 Source Addresses
namespace SA {
    constexpr uint8_t ENGINE        = 0x00;
    constexpr uint8_t TRANSMISSION  = 0x03;
    constexpr uint8_t INSTRUMENT    = 0x17;
    constexpr uint8_t BROADCAST     = 0xFF;
}

// Core message structure — represents one J1939 CAN frame
struct J1939Message {
    uint32_t timestamp_ms   = 0;
    uint32_t pgn            = 0;
    uint8_t  source_address = 0;
    uint8_t  dest_address   = 0xFF;
    uint8_t  length         = 0;
    std::array<uint8_t, 8> data = {};

    // Human readable string for console output
    std::string to_string() const {
        std::ostringstream oss;
        oss << "[" << std::setw(8) << timestamp_ms << "ms] "
            << "PGN=0x" << std::hex << std::uppercase << std::setw(4) << pgn
            << " SA=0x" << std::setw(2) << std::setfill('0') << static_cast<int>(source_address)
            << " Data=";
        for (int i = 0; i < length; i++) {
            oss << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(data[i]) << " ";
        }
        return oss.str();
    }

    // Check if message is valid
    bool is_valid() const {
        return pgn != 0 && length > 0 && length <= 8;
    }
};

} // namespace nexlog