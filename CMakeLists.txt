cmake_minimum_required(VERSION 3.20)
project(nexlog VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Common include directory
include_directories(${CMAKE_SOURCE_DIR})

# Parser library
add_library(parser_lib
    parser/asc_parser.cpp
    parser/hlog_parser.cpp
    parser/parser_factory.cpp
)

# Analyzer library
add_library(analyzer_lib
    analyzer/anomaly_detector.cpp
    analyzer/alert_manager.cpp
)

# Streamer library
add_library(streamer_lib
    streamer/udp_sender.cpp
)

# Nexlog diagnostic tool
add_executable(nexlog
    app/main.cpp
)

target_link_libraries(nexlog
    parser_lib
    analyzer_lib
    streamer_lib
)

# Windows specific — link Winsock for UDP
if(WIN32)
    target_link_libraries(nexlog ws2_32)
endif()