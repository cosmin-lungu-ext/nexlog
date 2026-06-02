#include "analyzer/anomaly_detector.hpp"
#include <sstream>

namespace nexlog {

std::string Anomaly::severity_str() const {
    switch (severity) {
        case Severity::INFO:     return "INFO";
        case Severity::WARNING:  return "WARNING";
        case Severity::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

std::string Anomaly::to_string() const {
    std::ostringstream oss;
    oss << "[" << severity_str() << "] "
        << signal_name << ": " << value << " " << unit
        << " (threshold: " << threshold << " " << unit << ") — "
        << description;
    return oss.str();
}

std::vector<Anomaly> AnomalyDetector::analyse(const J1939Message& msg) {
    std::vector<Anomaly> result;

    auto decoded = SignalDecoder::decode(msg);
    if (!decoded.has_value() || !decoded->valid) return result;

    const auto& signal = decoded.value();

    if (signal.name == "Engine Oil Pressure")   check_oil_pressure(signal, msg, result);
    if (signal.name == "Vehicle Speed")         check_vehicle_speed(signal, msg, result);
    if (signal.name == "Engine Coolant Temp")   check_engine_temp(signal, msg, result);
    if (signal.name == "Total Engine Hours")    check_engine_hours(signal, msg, result);

    // Store in history
    for (const auto& a : result) anomalies_.push_back(a);

    return result;
}

void AnomalyDetector::check_oil_pressure(const DecodedSignal& signal,
                                          const J1939Message& msg,
                                          std::vector<Anomaly>& result) {
    if (signal.value < 100.0) {
        Anomaly a;
        a.timestamp_ms = msg.timestamp_ms;
        a.signal_name  = signal.name;
        a.value        = signal.value;
        a.unit         = signal.unit;
        a.threshold    = 100.0;
        a.severity     = signal.value < 50.0 ? Severity::CRITICAL : Severity::WARNING;
        a.description  = "Low engine oil pressure detected";
        result.push_back(a);
    }
}

void AnomalyDetector::check_vehicle_speed(const DecodedSignal& signal,
                                           const J1939Message& msg,
                                           std::vector<Anomaly>& result) {
    if (signal.value > 120.0) {
        Anomaly a;
        a.timestamp_ms = msg.timestamp_ms;
        a.signal_name  = signal.name;
        a.value        = signal.value;
        a.unit         = signal.unit;
        a.threshold    = 120.0;
        a.severity     = Severity::WARNING;
        a.description  = "Vehicle speed exceeds limit";
        result.push_back(a);
    }
}

void AnomalyDetector::check_engine_temp(const DecodedSignal& signal,
                                         const J1939Message& msg,
                                         std::vector<Anomaly>& result) {
    if (signal.value > 105.0) {
        Anomaly a;
        a.timestamp_ms = msg.timestamp_ms;
        a.signal_name  = signal.name;
        a.value        = signal.value;
        a.unit         = signal.unit;
        a.threshold    = 105.0;
        a.severity     = signal.value > 115.0 ? Severity::CRITICAL : Severity::WARNING;
        a.description  = "Engine coolant temperature too high";
        result.push_back(a);
    }
}

void AnomalyDetector::check_engine_hours(const DecodedSignal& signal,
                                          const J1939Message& msg,
                                          std::vector<Anomaly>& result) {
    if (signal.value > 10000.0) {
        Anomaly a;
        a.timestamp_ms = msg.timestamp_ms;
        a.signal_name  = signal.name;
        a.value        = signal.value;
        a.unit         = signal.unit;
        a.threshold    = 10000.0;
        a.severity     = Severity::INFO;
        a.description  = "Engine approaching service interval";
        result.push_back(a);
    }
}

} // namespace nexlog