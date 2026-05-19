#pragma once

#include "common/Models.h"
#include "common/ThreadSafeQueue.h"

#include <atomic>

namespace nids {

class IPacketCapture {
public:
    virtual ~IPacketCapture() = default;
    virtual void run(ThreadSafeQueue<RawPacket>& output, std::atomic_bool& running) = 0;
};

} // namespace nids

