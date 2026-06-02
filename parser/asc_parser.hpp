#pragma once

#include "parser/abstract_parser.hpp"
#include <fstream>
#include <string>

namespace nexlog {

class ASCParser : public AbstractParser {
public:
    ASCParser() = default;
    ~ASCParser() override;

    bool open(const std::string& path) override;
    bool has_next() override;
    J1939Message next() override;
    void close() override;
    int message_count() const override;
    std::string file_path() const override;

private:
    std::ifstream   file_;
    std::string     file_path_;
    std::string     next_line_;
    int             message_count_ = 0;
    bool            has_next_      = false;

    // Parse one line from ASC file into a J1939Message
    bool parse_line(const std::string& line, J1939Message& msg);

    // Advance to next valid message line
    void advance();

    // Check if line is a data line — not a comment or header
    bool is_data_line(const std::string& line) const;
};

} // namespace nexlog