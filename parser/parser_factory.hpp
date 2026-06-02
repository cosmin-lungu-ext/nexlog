#pragma once

#include "parser/abstract_parser.hpp"
#include <memory>
#include <string>

namespace nexlog {

class ParserFactory {
public:
    // Create the correct parser based on file extension
    // Returns nullptr if extension not supported
    static std::unique_ptr<AbstractParser> create(const std::string& filepath);

private:
    static std::string get_extension(const std::string& filepath);
};

} // namespace nexlog