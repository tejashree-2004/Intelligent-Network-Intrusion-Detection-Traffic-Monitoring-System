#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace nids {

using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

struct RawPacket {
    std::string sourceIp;
    std::string destinationIp;
    uint16_t sourcePort = 0;
    uint16_t destinationPort = 0;
    std::string protocol;
    std::size_t sizeBytes = 0;
    TimePoint timestamp = Clock::now();
};

struct ParsedPacket {
    std::string sourceIp;
    std::string destinationIp;
    uint16_t sourcePort = 0;
    uint16_t destinationPort = 0;
    std::string protocol;
    std::size_t sizeBytes = 0;
    TimePoint timestamp = Clock::now();
};

struct TrafficFlow {
    std::string sourceIp;
    std::string destinationIp;
    uint16_t sourcePort = 0;
    uint16_t destinationPort = 0;
    std::string protocol;
    std::uint64_t packetCount = 0;
    std::uint64_t bytesTransferred = 0;
    double durationSeconds = 0.0;
    TimePoint firstSeen = Clock::now();
    TimePoint lastSeen = Clock::now();
};

struct Alert {
    std::string alertType;
    std::string severity;
    std::string sourceIp;
    std::string description;
    TimePoint createdAt = Clock::now();
};

struct SystemStats {
    std::uint64_t activeConnections = 0;
    std::uint64_t totalPackets = 0;
    std::uint64_t anomaliesDetected = 0;
};

std::string toIso8601(TimePoint timePoint);

} // namespace nids

