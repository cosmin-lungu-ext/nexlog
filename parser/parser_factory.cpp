#include "parser/parser_factory.hpp"
#include "parser/asc_parser.hpp"
#include "parser/hlog_parser.hpp"
#include <algorithm>
#include <iostream>

namespace nexlog {

std::unique_ptr<AbstractParser> ParserFactory::create(const std::string& filepath) {
    std::string ext = get_extension(filepath);

    // Convert to lowercase for comparison
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (ext == ".asc") {
        std::cout << "ParserFactory: creating ASCParser for " << filepath << std::endl;
        return std::make_unique<ASCParser>();
    } else if (ext == ".hlog") {
        std::cout << "ParserFactory: creating HLOGParser for " << filepath << std::endl;
        return std::make_unique<HLOGParser>();
    }

    std::cerr << "ParserFactory: unsupported file extension: " << ext << std::endl;
    return nullptr;
}

std::string ParserFactory::get_extension(const std::string& filepath) {
    size_t pos = filepath.rfind('.');
    if (pos == std::string::npos) return "";
    return filepath.substr(pos);
}

} // namespace nexlog