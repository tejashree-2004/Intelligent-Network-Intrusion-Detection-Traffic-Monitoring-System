#pragma once

#include "packet-capture/IPacketCapture.h"

namespace nids {

class MockPacketCapture final : public IPacketCapture {
public:
    void run(ThreadSafeQueue<RawPacket>& output, std::atomic_bool& running) override;
};

} // namespace nids

