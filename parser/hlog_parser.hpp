#pragma once

#include "parser/abstract_parser.hpp"
#include <fstream>
#include <string>

namespace nexlog {

class HLOGParser : public AbstractParser {
public:
    HLOGParser() = default;
    ~HLOGParser() override;

    bool open(const std::string& path) override;
    bool has_next() override;
    J1939Message next() override;
    void close() override;
    int message_count() const override;
    std::string file_path() const override;

private:
    std::ifstream file_;
    std::string   file_path_;
    std::string   next_line_;
    int           message_count_ = 0;
    bool          has_next_      = false;
    bool          header_parsed_ = false;

    void advance();
    bool is_data_line(const std::string& line) const;
    bool parse_line(const std::string& line, J1939Message& msg);
};

} // namespace nexlog