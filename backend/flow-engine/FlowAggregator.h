#pragma once

#include "common/Models.h"

#include <map>
#include <string>

namespace nids {

class FlowAggregator {
public:
    TrafficFlow update(const ParsedPacket& packet);
    std::size_t activeConnectionCount() const;

private:
    std::map<std::string, TrafficFlow> flows_;

    static std::string keyFor(const ParsedPacket& packet);
};

} // namespace nids
