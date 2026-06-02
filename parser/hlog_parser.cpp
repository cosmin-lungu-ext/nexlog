#include "parser/hlog_parser.hpp"
#include <sstream>
#include <iostream>

namespace nexlog {

HLOGParser::~HLOGParser() {
    close();
}

bool HLOGParser::open(const std::string& path) {
    file_path_ = path;
    file_.open(path);

    if (!file_.is_open()) {
        std::cerr << "HLOGParser: failed to open file: " << path << std::endl;
        return false;
    }

    // Skip header section
    std::string line;
    while (std::getline(file_, line)) {
        if (line.empty() || line[0] == '[' || line[0] == '#') continue;
        // First non-header line found
        if (is_data_line(line)) {
            next_line_ = line;
            has_next_  = true;
            return true;
        }
    }

    advance();
    return true;
}

bool HLOGParser::has_next() {
    return has_next_;
}

J1939Message HLOGParser::next() {
    J1939Message msg;
    parse_line(next_line_, msg);
    message_count_++;
    advance();
    return msg;
}

void HLOGParser::close() {
    if (file_.is_open()) file_.close();
    has_next_ = false;
}

int HLOGParser::message_count() const {
    return message_count_;
}

std::string HLOGParser::file_path() const {
    return file_path_;
}

void HLOGParser::advance() {
    std::string line;
    while (std::getline(file_, line)) {
        if (is_data_line(line)) {
            next_line_ = line;
            has_next_  = true;
            return;
        }
    }
    has_next_ = false;
}

bool HLOGParser::is_data_line(const std::string& line) const {
    if (line.empty())   return false;
    if (line[0] == '#') return false;  // comment
    if (line[0] == '[') return false;  // header section
    // Data lines start with a digit — timestamp
    return std::isdigit(static_cast<unsigned char>(line[0]));
}

bool HLOGParser::parse_line(const std::string& line, J1939Message& msg) {
    // Format: timestamp | PGN | SA | DA | Len | Data
    // Example: 1000 | 0xFEEF | 0x00 | 0xFF | 8 | 3C 00 FF FF FF FF FF FF
    std::istringstream iss(line);
    std::string token;

    // Timestamp
    std::getline(iss, token, '|');
    msg.timestamp_ms = static_cast<uint32_t>(std::stoul(token));

    // PGN
    std::getline(iss, token, '|');
    msg.pgn = static_cast<uint32_t>(std::stoul(token, nullptr, 16));

    // Source address
    std::getline(iss, token, '|');
    msg.source_address = static_cast<uint8_t>(std::stoul(token, nullptr, 16));

    // Dest address
    std::getline(iss, token, '|');
    msg.dest_address = static_cast<uint8_t>(std::stoul(token, nullptr, 16));

    // Length
    std::getline(iss, token, '|');
    msg.length = static_cast<uint8_t>(std::stoul(token));

    // Data bytes
    std::getline(iss, token);
    std::istringstream data_iss(token);
    std::string byte_str;
    int i = 0;
    while (data_iss >> byte_str && i < msg.length) {
        msg.data[i++] = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
    }

    return true;
}

} // namespace nexlog