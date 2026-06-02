#include "parser/asc_parser.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nexlog {

ASCParser::~ASCParser() {
    close();
}

bool ASCParser::open(const std::string& path) {
    file_path_ = path;
    file_.open(path);

    if (!file_.is_open()) {
        std::cerr << "ASCParser: failed to open file: " << path << std::endl;
        return false;
    }

    // Advance to first valid message
    advance();
    return true;
}

bool ASCParser::has_next() {
    return has_next_;
}

J1939Message ASCParser::next() {
    J1939Message msg;

    // Parse current line into message
    parse_line(next_line_, msg);
    message_count_++;

    // Advance to next valid line
    advance();

    return msg;
}

void ASCParser::close() {
    if (file_.is_open()) {
        file_.close();
    }
    has_next_ = false;
}

int ASCParser::message_count() const {
    return message_count_;
}

std::string ASCParser::file_path() const {
    return file_path_;
}

// Advance to next valid data line
void ASCParser::advance() {
    std::string line;
    while (std::getline(file_, line)) {
        if (is_data_line(line)) {
            next_line_ = line;
            has_next_  = true;
            return;
        }
    }
    // No more lines
    has_next_ = false;
}

// Check if line contains CAN data — not comment or header
bool ASCParser::is_data_line(const std::string& line) const {
    if (line.empty())            return false;
    if (line[0] == '/')          return false;  // comment
    if (line[0] == 'd')          return false;  // "date" header
    if (line[0] == 'b')          return false;  // "base" header
    if (line[0] == 'i')          return false;  // "internal" header
    if (line[0] == 'e')          return false;  // "end"

    // Data lines start with a timestamp — first char is digit or space
    return std::isdigit(static_cast<unsigned char>(line[0])) ||
           line[0] == ' ';
}

// Parse one ASC line into J1939Message
// Format: timestamp channel id Rx/Tx d length byte0 byte1 ...
// Example: 0.000000 1 18FEEF00x Rx d 8 3C 00 FF FF FF FF FF FF
bool ASCParser::parse_line(const std::string& line, J1939Message& msg) {
    std::istringstream iss(line);

    double      timestamp_sec = 0.0;
    int         channel       = 0;
    std::string id_str;
    std::string direction;
    std::string d_char;
    int         length        = 0;

    // Parse fixed fields
    if (!(iss >> timestamp_sec >> channel >> id_str >> direction >> d_char >> length)) {
        return false;
    }

    // Convert timestamp to milliseconds
    msg.timestamp_ms = static_cast<uint32_t>(timestamp_sec * 1000.0);

    // Remove trailing 'x' from CAN ID if present
    // 18FEEF00x → 18FEEF00
    if (!id_str.empty() && id_str.back() == 'x') {
        id_str.pop_back();
    }

    // Parse CAN ID as hex — extract PGN, SA from 29-bit extended ID
    // J1939 29-bit ID format:
    // bits 28-26: priority (3 bits)
    // bits 25-24: reserved + data page (2 bits)
    // bits 23-16: PGN high byte (8 bits)
    // bits 15-8:  PGN low byte (8 bits) — for peer to peer PDU1
    // bits 7-0:   source address (8 bits)
    uint32_t can_id = std::stoul(id_str, nullptr, 16);

    msg.source_address = static_cast<uint8_t>(can_id & 0xFF);
    msg.pgn            = (can_id >> 8) & 0x3FFFF;  // 18 bits
    msg.dest_address   = 0xFF;                       // broadcast default
    msg.length         = static_cast<uint8_t>(std::min(length, 8));

    // Parse data bytes
    for (int i = 0; i < msg.length; i++) {
        std::string byte_str;
        if (iss >> byte_str) {
            msg.data[i] = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
        }
    }

    return true;
}

} // namespace nexlog