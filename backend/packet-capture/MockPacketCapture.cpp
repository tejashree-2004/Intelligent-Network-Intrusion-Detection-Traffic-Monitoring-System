#include "packet-capture/MockPacketCapture.h"

#include <chrono>
#include <random>
#include <thread>

namespace nids {

void MockPacketCapture::run(ThreadSafeQueue<RawPacket>& output, std::atomic_bool& running) {
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> host(2, 250);
    std::uniform_int_distribution<int> port(20, 9000);
    std::uniform_int_distribution<int> size(64, 1500);
    std::uniform_int_distribution<int> protocol(0, 10);

    while (running.load()) {
        RawPacket packet;
        packet.sourceIp = protocol(rng) > 7 ? "192.168.1.50" : "192.168.1." + std::to_string(host(rng));
        packet.destinationIp = "10.0.0." + std::to_string(host(rng));
        packet.sourcePort = static_cast<uint16_t>(port(rng));
        packet.destinationPort = protocol(rng) > 8 ? static_cast<uint16_t>(port(rng)) : 443;
        packet.protocol = protocol(rng) == 0 ? "UDP" : "TCP";
        packet.sizeBytes = static_cast<std::size_t>(size(rng));
        packet.timestamp = Clock::now();
        output.push(packet);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

} // namespace nids

