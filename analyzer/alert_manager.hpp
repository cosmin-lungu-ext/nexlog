#pragma once

#include "analyzer/anomaly_detector.hpp"
#include <vector>
#include <string>
#include <functional>

namespace nexlog {

class AlertManager {
public:
    // Callback type — called when anomaly is detected
    using AlertCallback = std::function<void(const Anomaly&)>;

    AlertManager() = default;

    // Register a callback for anomaly alerts
    void register_callback(AlertCallback callback);

    // Process anomalies — triggers callbacks
    void process(const std::vector<Anomaly>& anomalies);

    // Get total alert count
    int alert_count() const { return alert_count_; }

private:
    std::vector<AlertCallback> callbacks_;
    int alert_count_ = 0;
};

} // namespace nexlog