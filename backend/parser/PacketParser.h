#pragma once

#include "common/Models.h"

namespace nids {

class PacketParser {
public:
    ParsedPacket parse(const RawPacket& packet) const;
};

} // namespace nids

