#include "flow-engine/FlowAggregator.h"

#include <chrono>
#include <sstream>

namespace nids {

TrafficFlow FlowAggregator::update(const ParsedPacket& packet) {
    auto key = keyFor(packet);
    auto found = flows_.find(key);

    if (found == flows_.end()) {
        TrafficFlow flow;
        flow.sourceIp = packet.sourceIp;
        flow.destinationIp = packet.destinationIp;
        flow.sourcePort = packet.sourcePort;
        flow.destinationPort = packet.destinationPort;
        flow.protocol = packet.protocol;
        flow.firstSeen = packet.timestamp;
        flow.lastSeen = packet.timestamp;
        found = flows_.emplace(key, flow).first;
    }

    TrafficFlow& flow = found->second;
    flow.packetCount += 1;
    flow.bytesTransferred += packet.sizeBytes;
    flow.lastSeen = packet.timestamp;
    flow.durationSeconds = std::chrono::duration<double>(flow.lastSeen - flow.firstSeen).count();
    return flow;
}

std::size_t FlowAggregator::activeConnectionCount() const {
    return flows_.size();
}

std::string FlowAggregator::keyFor(const ParsedPacket& packet) {
    std::ostringstream key;
    key << packet.sourceIp << ':'
        << packet.sourcePort << "->"
        << packet.destinationIp << ':'
        << packet.destinationPort << '/'
        << packet.protocol;
    return key.str();
}

} // namespace nids

