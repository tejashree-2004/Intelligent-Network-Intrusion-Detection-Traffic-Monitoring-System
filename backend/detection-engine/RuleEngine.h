#pragma once

#include "common/Models.h"

#include <map>
#include <set>
#include <string>

namespace nids {

class RuleEngine {
public:
    bool evaluate(const TrafficFlow& flow, Alert& alert);

private:
    std::map<std::string, std::set<uint16_t>> portsBySource_;
    std::map<std::string, std::uint64_t> packetsBySource_;
};

} // namespace nids
