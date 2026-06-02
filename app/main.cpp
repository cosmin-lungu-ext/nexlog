#include <iostream>
#include <string>
#include <memory>
#include "common/message.hpp"
#include "common/signal_decoder.hpp"
#include "parser/parser_factory.hpp"
#include "analyzer/anomaly_detector.hpp"
#include "analyzer/alert_manager.hpp"
#include "streamer/udp_sender.hpp"

void print_usage() {
    std::cout << "Usage: nexlog <logfile> [--stream <host> <port>]" << std::endl;
    std::cout << "  logfile:  path to .asc or .hlog file" << std::endl;
    std::cout << "  --stream: optional UDP streaming to server" << std::endl;
    std::cout << "Example: nexlog data/sample.asc" << std::endl;
    std::cout << "Example: nexlog data/sample.asc --stream 127.0.0.1 9000" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "╔══════════════════════════════╗" << std::endl;
    std::cout << "║   nexlog diagnostic tool     ║" << std::endl;
    std::cout << "║   CAN/J1939 log analyzer     ║" << std::endl;
    std::cout << "╚══════════════════════════════╝" << std::endl;

    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string filepath = argv[1];

    // Optional streaming
    bool streaming = false;
    std::string stream_host;
    uint16_t stream_port = 0;

    if (argc >= 5 && std::string(argv[2]) == "--stream") {
        stream_host = argv[3];
        stream_port = static_cast<uint16_t>(std::stoi(argv[4]));
        streaming   = true;
    }

    // Create parser via factory
    auto parser = nexlog::ParserFactory::create(filepath);
    if (!parser) {
        std::cerr << "Failed to create parser for: " << filepath << std::endl;
        return 1;
    }

    if (!parser->open(filepath)) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return 1;
    }

    // Setup anomaly detector and alert manager
    nexlog::AnomalyDetector detector;
    nexlog::AlertManager    alert_manager;

    // Register alert callback — lambda function
    alert_manager.register_callback([](const nexlog::Anomaly& anomaly) {
        std::cout << "  ⚠ " << anomaly.to_string() << std::endl;
    });

    // Setup UDP streamer if requested
    nexlog::UDPSender sender;
    if (streaming) {
        if (!sender.init(stream_host, stream_port)) {
            std::cerr << "Failed to init UDP streamer" << std::endl;
            return 1;
        }
    }

    std::cout << "\nParsing: " << filepath << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    // Main processing loop
    while (parser->has_next()) {
        nexlog::J1939Message msg = parser->next();

        // Print message
        std::cout << msg.to_string() << std::endl;

        // Decode signal
        auto decoded = nexlog::SignalDecoder::decode(msg);
        if (decoded.has_value() && decoded->valid) {
            std::cout << "  → " << decoded->to_string() << std::endl;
        }

        // Detect anomalies
        auto anomalies = detector.analyse(msg);
        alert_manager.process(anomalies);

        // Stream via UDP if enabled
        if (streaming && sender.is_connected()) {
            nexlog::DiagPacket packet;
            packet.timestamp_ms = msg.timestamp_ms;
            packet.pgn          = msg.pgn;
            packet.source_addr  = msg.source_address;
            packet.dest_addr    = msg.dest_address;
            packet.length       = msg.length;
            std::memcpy(packet.data, msg.data.data(), msg.length);
            packet.checksum     = packet.calculate_checksum();
            sender.send(packet);
        }
    }

    std::cout << std::string(50, '-') << std::endl;
    std::cout << "Total messages: " << parser->message_count() << std::endl;
    std::cout << "Total alerts:   " << alert_manager.alert_count() << std::endl;

    return 0;
}