#pragma once

#include "common/message.hpp"
#include "common/signal_decoder.hpp"
#include <string>
#include <vector>

namespace nexlog {

// Severity level of an anomaly
enum class Severity {
    INFO,
    WARNING,
    CRITICAL
};

// Represents a detected anomaly
struct Anomaly {
    uint32_t    timestamp_ms = 0;
    std::string signal_name;
    double      value        = 0.0;
    std::string unit;
    double      threshold    = 0.0;
    Severity    severity     = Severity::INFO;
    std::string description;

    std::string to_string() const;
    std::string severity_str() const;
};

class AnomalyDetector {
public:
    AnomalyDetector() = default;

    // Analyse a message — returns anomaly if threshold exceeded
    std::vector<Anomaly> analyse(const J1939Message& msg);

    // Get all detected anomalies
    const std::vector<Anomaly>& anomalies() const { return anomalies_; }

    // Clear anomaly history
    void clear() { anomalies_.clear(); }

private:
    std::vector<Anomaly> anomalies_;

    // Threshold checks per signal
    void check_oil_pressure(const DecodedSignal& signal,
                             const J1939Message& msg,
                             std::vector<Anomaly>& result);

    void check_vehicle_speed(const DecodedSignal& signal,
                              const J1939Message& msg,
                              std::vector<Anomaly>& result);

    void check_engine_temp(const DecodedSignal& signal,
                            const J1939Message& msg,
                            std::vector<Anomaly>& result);

    void check_engine_hours(const DecodedSignal& signal,
                             const J1939Message& msg,
                             std::vector<Anomaly>& result);
};

} // namespace nexlog