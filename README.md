# Intelligent Network Intrusion Detection & Traffic Monitoring System

Phase 1 implements a real-time backend foundation:

- C++ packet capture and multithreaded processing service
- Thread-safe queues across capture, parse, flow aggregation, detection, and alert processing
- Rule-based detection for port scans, traffic floods, and suspicious protocol usage
- SQLite persistence for flows, alerts, and system statistics
- REST API for statistics, alerts, suspicious IPs, and block requests
- Mock packet mode for development on Windows/macOS and libpcap mode for Linux

## Project Layout

```text
backend/
  packet-capture/      Packet capture adapters
  parser/              Packet parsing helpers
  flow-engine/         Flow aggregation
  detection-engine/    Rule detection
  api/                 REST API service
  common/              Shared models, queues, database helpers
database/
  schema.sql
docs/
  phase-1.md
ml/
  training/
  models/
dashboard/
```

## Quick Start: API

```powershell
cd backend/api
npm install
npm start
```

API base URL: `http://localhost:3000`

Available endpoints:

- `GET /api/traffic/stats`
- `GET /api/alerts`
- `GET /api/suspicious`
- `POST /api/block`

## Quick Start: C++ Service

The service has a mock packet generator enabled by default, so it can run without administrator packet capture permissions.

```bash
cmake -S backend -B build
cmake --build build
./build/nids_service --mock
```

On Linux with libpcap installed:

```bash
cmake -S backend -B build -DENABLE_PCAP=ON
cmake --build build
sudo ./build/nids_service --interface eth0
```

## Phase Roadmap

1. Packet capture, multithreaded processing, flow aggregation, REST APIs
2. Isolation Forest anomaly model and model-serving bridge
3. Dashboard with live traffic cards, charts, alerts, and suspicious IPs
4. Docker deployment and optional WebSocket updates

