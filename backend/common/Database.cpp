#include "common/Database.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include <ctime>
#include <iomanip>
#include <sstream>
#include <utility>

namespace nids {

std::string toIso8601(TimePoint timePoint) {
    const auto time = Clock::to_time_t(timePoint);
    std::tm tm{};
#if defined(_POSIX_VERSION)
    gmtime_r(&time, &tm);
#else
    if (std::tm* converted = std::gmtime(&time)) {
        tm = *converted;
    }
#endif
    std::ostringstream stream;
    stream << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return stream.str();
}

Database::Database(std::string outputDirectory)
    : outputDirectory_(std::move(outputDirectory)) {
#ifdef _WIN32
    _mkdir(outputDirectory_.c_str());
#else
    mkdir(outputDirectory_.c_str(), 0755);
#endif
}

void Database::saveFlow(const TrafficFlow& flow) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto file = openAppend("traffic_flows.csv");
    file << toIso8601(flow.lastSeen) << ','
         << flow.sourceIp << ','
         << flow.destinationIp << ','
         << flow.sourcePort << ','
         << flow.destinationPort << ','
         << flow.protocol << ','
         << flow.packetCount << ','
         << flow.bytesTransferred << ','
         << flow.durationSeconds << '\n';
}

void Database::saveAlert(const Alert& alert) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto file = openAppend("alerts.csv");
    file << toIso8601(alert.createdAt) << ','
         << alert.alertType << ','
         << alert.severity << ','
         << alert.sourceIp << ','
         << '"' << alert.description << '"' << '\n';
}

void Database::saveStats(const SystemStats& stats) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto file = openAppend("system_stats.csv");
    file << toIso8601(Clock::now()) << ','
         << stats.activeConnections << ','
         << stats.totalPackets << ','
         << stats.anomaliesDetected << '\n';
}

std::ofstream Database::openAppend(const std::string& fileName) {
    const std::string separator =
#ifdef _WIN32
        "\\";
#else
        "/";
#endif
    return std::ofstream(outputDirectory_ + separator + fileName, std::ios::app);
}

} // namespace nids
