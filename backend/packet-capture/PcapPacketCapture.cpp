#ifdef ENABLE_PCAP

#include "packet-capture/IPacketCapture.h"

#include <pcap.h>
#include <stdexcept>
#include <string>

namespace nids {

class PcapPacketCapture final : public IPacketCapture {
public:
    explicit PcapPacketCapture(std::string interfaceName)
        : interfaceName_(std::move(interfaceName)) {}

    void run(ThreadSafeQueue<RawPacket>& output, std::atomic_bool& running) override {
        char errorBuffer[PCAP_ERRBUF_SIZE]{};
        pcap_t* handle = pcap_open_live(interfaceName_.c_str(), BUFSIZ, 1, 1000, errorBuffer);
        if (!handle) {
            throw std::runtime_error(errorBuffer);
        }

        while (running.load()) {
            pcap_pkthdr* header = nullptr;
            const u_char* data = nullptr;
            const int result = pcap_next_ex(handle, &header, &data);
            if (result <= 0 || header == nullptr || data == nullptr) {
                continue;
            }

            RawPacket packet;
            packet.sourceIp = "0.0.0.0";
            packet.destinationIp = "0.0.0.0";
            packet.protocol = "RAW";
            packet.sizeBytes = header->len;
            packet.timestamp = Clock::now();
            output.push(packet);
        }

        pcap_close(handle);
    }

private:
    std::string interfaceName_;
};

} // namespace nids

#endif

