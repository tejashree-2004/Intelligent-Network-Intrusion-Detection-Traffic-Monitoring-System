#include "common/Database.h"
#include "common/Models.h"
#include "common/ThreadSafeQueue.h"
#include "detection-engine/RuleEngine.h"
#include "flow-engine/FlowAggregator.h"
#include "packet-capture/MockPacketCapture.h"
#include "parser/PacketParser.h"

#include <atomic>
#include <csignal>
#include <iostream>
#include <thread>

using namespace nids;

namespace {
std::atomic_bool running{true};

void handleSignal(int) {
    running.store(false);
}
}

int main(int argc, char** argv) {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    ThreadSafeQueue<RawPacket> rawPackets;
    ThreadSafeQueue<ParsedPacket> parsedPackets;
    ThreadSafeQueue<TrafficFlow> flows;
    ThreadSafeQueue<Alert> alerts;

    Database database{"runtime-data"};
    SystemStats stats;

    MockPacketCapture capture;
    PacketParser parser;
    FlowAggregator aggregator;
    RuleEngine rules;

    std::thread captureThread([&] {
        capture.run(rawPackets, running);
        rawPackets.stop();
    });

    std::thread parserThread([&] {
        RawPacket packet;
        while (rawPackets.waitPop(packet)) {
            parsedPackets.push(parser.parse(packet));
            stats.totalPackets += 1;
        }
        parsedPackets.stop();
    });

    std::thread flowThread([&] {
        ParsedPacket packet;
        while (parsedPackets.waitPop(packet)) {
            auto flow = aggregator.update(packet);
            stats.activeConnections = aggregator.activeConnectionCount();
            flows.push(flow);
            database.saveFlow(flow);
        }
        flows.stop();
    });

    std::thread detectionThread([&] {
        TrafficFlow flow;
        while (flows.waitPop(flow)) {
            Alert alert;
            if (rules.evaluate(flow, alert)) {
                stats.anomaliesDetected += 1;
                alerts.push(alert);
            }
            database.saveStats(stats);
        }
        alerts.stop();
    });

    std::thread alertThread([&] {
        Alert alert;
        while (alerts.waitPop(alert)) {
            database.saveAlert(alert);
            std::cout << '[' << alert.severity << "] "
                      << alert.alertType << " from "
                      << alert.sourceIp << ": "
                      << alert.description << '\n';
        }
    });

    std::cout << "NIDS service running in mock capture mode. Press Ctrl+C to stop.\n";

    captureThread.join();
    parserThread.join();
    flowThread.join();
    detectionThread.join();
    alertThread.join();

    return 0;
}
