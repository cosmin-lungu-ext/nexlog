#include "analyzer/alert_manager.hpp"

namespace nexlog {

void AlertManager::register_callback(AlertCallback callback) {
    callbacks_.push_back(callback);
}

void AlertManager::process(const std::vector<Anomaly>& anomalies) {
    for (const auto& anomaly : anomalies) {
        alert_count_++;
        for (const auto& callback : callbacks_) {
            callback(anomaly);
        }
    }
}

} // namespace nexlog