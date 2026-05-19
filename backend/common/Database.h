#pragma once

#include "common/Models.h"

#include <fstream>
#include <mutex>
#include <string>

namespace nids {

class Database {
public:
    explicit Database(std::string outputDirectory);

    void saveFlow(const TrafficFlow& flow);
    void saveAlert(const Alert& alert);
    void saveStats(const SystemStats& stats);

private:
    std::string outputDirectory_;
    std::mutex mutex_;

    std::ofstream openAppend(const std::string& fileName);
};

} // namespace nids

