#include "parser/PacketParser.h"

namespace nids {

ParsedPacket PacketParser::parse(const RawPacket& packet) const {
    return ParsedPacket{
        packet.sourceIp,
        packet.destinationIp,
        packet.sourcePort,
        packet.destinationPort,
        packet.protocol,
        packet.sizeBytes,
        packet.timestamp
    };
}

} // namespace nids

