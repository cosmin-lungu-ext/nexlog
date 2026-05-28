#pragma once

#include "common/message.hpp"
#include <cstdint>
#include <optional>
#include <string>

namespace nexlog {

// Decoded engineering values from a J1939 message
struct DecodedSignal {
    std::string name;
    double      value  = 0.0;
    std::string unit;
    bool        valid  = false;

    std::string to_string() const {
        if (!valid) return name + ": INVALID";
        return name + ": " + std::to_string(value) + " " + unit;
    }
};

class SignalDecoder {
public:

    // Decode a J1939 message into engineering values
    // Returns empty optional if PGN is not supported
    static std::optional<DecodedSignal> decode(const J1939Message& msg) {
        switch (msg.pgn) {
            case PGN::ENGINE_FLUID_PRESSURE:
                return decode_engine_pressure(msg);
            case PGN::VEHICLE_SPEED:
                return decode_vehicle_speed(msg);
            case PGN::ENGINE_HOURS:
                return decode_engine_hours(msg);
            case PGN::ENGINE_TEMP:
                return decode_engine_temp(msg);
            default:
                return std::nullopt;  // unsupported PGN
        }
    }

private:

    // Engine Oil Pressure — byte 3 — scale: 4 kPa/bit, offset: 0
    static DecodedSignal decode_engine_pressure(const J1939Message& msg) {
        DecodedSignal signal;
        signal.name  = "Engine Oil Pressure";
        signal.unit  = "kPa";

        uint8_t raw = msg.data[3];
        if (raw == 0xFF) {
            signal.valid = false;  // 0xFF means not available in J1939
            return signal;
        }

        signal.value = raw * 4.0;  // scale factor: 4 kPa per bit
        signal.valid = true;
        return signal;
    }

    // Vehicle Speed — bytes 0-1 — scale: 1/256 km/h per bit
    static DecodedSignal decode_vehicle_speed(const J1939Message& msg) {
        DecodedSignal signal;
        signal.name = "Vehicle Speed";
        signal.unit = "km/h";

        // Two bytes combined — little endian
        uint16_t raw = static_cast<uint16_t>(msg.data[0]) |
                       static_cast<uint16_t>(msg.data[1]) << 8;

        if (raw == 0xFFFF) {
            signal.valid = false;
            return signal;
        }

        signal.value = raw / 256.0;  // scale factor
        signal.valid = true;
        return signal;
    }

    // Engine Hours — bytes 0-3 — scale: 0.05 hours per bit
    static DecodedSignal decode_engine_hours(const J1939Message& msg) {
        DecodedSignal signal;
        signal.name = "Total Engine Hours";
        signal.unit = "h";

        // Four bytes combined — little endian
        uint32_t raw = static_cast<uint32_t>(msg.data[0])        |
                       static_cast<uint32_t>(msg.data[1]) << 8   |
                       static_cast<uint32_t>(msg.data[2]) << 16  |
                       static_cast<uint32_t>(msg.data[3]) << 24;

        if (raw == 0xFFFFFFFF) {
            signal.valid = false;
            return signal;
        }

        signal.value = raw * 0.05;
        signal.valid = true;
        return signal;
    }

    // Engine Coolant Temperature — byte 0 — scale: 1 deg/bit, offset: -40
    static DecodedSignal decode_engine_temp(const J1939Message& msg) {
        DecodedSignal signal;
        signal.name = "Engine Coolant Temp";
        signal.unit = "°C";

        uint8_t raw = msg.data[0];
        if (raw == 0xFF) {
            signal.valid = false;
            return signal;
        }

        signal.value = static_cast<double>(raw) - 40.0;  // offset: -40
        signal.valid = true;
        return signal;
    }
};

} // namespace nexlog