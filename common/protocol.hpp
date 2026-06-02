#pragma once

#include <cstdint>
#include <cstring>

namespace nexlog {

// UDP packet format for streaming J1939 messages to server
#pragma pack(push, 1)
struct DiagPacket {
    uint32_t magic        = 0xCAFEBABE;  // identification
    uint32_t timestamp_ms = 0;
    uint32_t pgn          = 0;
    uint8_t  source_addr  = 0;
    uint8_t  dest_addr    = 0xFF;
    uint8_t  length       = 0;
    uint8_t  data[8]      = {};
    uint16_t checksum     = 0;

    // Simple checksum — sum of all bytes except checksum field
    uint16_t calculate_checksum() const {
        uint16_t sum = 0;
        sum += (timestamp_ms & 0xFF) + ((timestamp_ms >> 8) & 0xFF) +
               ((timestamp_ms >> 16) & 0xFF) + ((timestamp_ms >> 24) & 0xFF);
        sum += (pgn & 0xFF) + ((pgn >> 8) & 0xFF) +
               ((pgn >> 16) & 0xFF) + ((pgn >> 24) & 0xFF);
        sum += source_addr + dest_addr + length;
        for (int i = 0; i < length; i++) sum += data[i];
        return sum;
    }
};
#pragma pack(pop)

} // namespace nexlog