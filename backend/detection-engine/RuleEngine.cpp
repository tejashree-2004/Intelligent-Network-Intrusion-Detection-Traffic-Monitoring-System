#include "detection-engine/RuleEngine.h"

namespace nids {

bool RuleEngine::evaluate(const TrafficFlow& flow, Alert& alert) {
    portsBySource_[flow.sourceIp].insert(flow.destinationPort);
    packetsBySource_[flow.sourceIp] += flow.packetCount;

    if (portsBySource_[flow.sourceIp].size() >= 25) {
        alert = Alert{
            "PORT_SCAN",
            "HIGH",
            flow.sourceIp,
            "Single source contacted many destination ports in a short monitoring window.",
            Clock::now()
        };
        return true;
    }

    if (flow.packetCount > 100 || packetsBySource_[flow.sourceIp] > 3000) {
        alert = Alert{
            "TRAFFIC_FLOOD",
            "CRITICAL",
            flow.sourceIp,
            "Request volume exceeded the configured traffic flood threshold.",
            Clock::now()
        };
        return true;
    }

    if (flow.protocol != "TCP" && flow.protocol != "UDP" && flow.protocol != "ICMP") {
        alert = Alert{
            "SUSPICIOUS_PROTOCOL",
            "MEDIUM",
            flow.sourceIp,
            "Unexpected protocol observed in captured traffic.",
            Clock::now()
        };
        return true;
    }

    return false;
}

} // namespace nids
