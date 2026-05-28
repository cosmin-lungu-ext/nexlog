#pragma once

#include "common/message.hpp"
#include <string>
#include <memory>

namespace nexlog {

// Abstract base class — defines the interface every parser must implement
// Concrete parsers: ASCParser, HLOGParser
class AbstractParser {
public:
    // Virtual destructor — MUST have when using polymorphism
    virtual ~AbstractParser() = default;

    // Open a log file — returns true if successful
    virtual bool open(const std::string& path) = 0;

    // Check if more messages are available
    virtual bool has_next() = 0;

    // Read and return the next message
    virtual J1939Message next() = 0;

    // Close the file
    virtual void close() = 0;

    // Get number of messages read so far
    virtual int message_count() const = 0;

    // Get the file path that was opened
    virtual std::string file_path() const = 0;
};

} // namespace nexlog