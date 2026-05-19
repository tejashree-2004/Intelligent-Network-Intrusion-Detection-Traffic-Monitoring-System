# Phase 1: Packet Capture, Processing, and REST APIs

## Implemented Scope

Phase 1 creates the core backend shape:

- Packet capture abstraction with a mock generator
- Five-thread processing pipeline:
  - packet capture
  - packet parsing
  - flow aggregation
  - detection
  - alert processing
- Rule engine for common suspicious patterns
- REST API with SQLite schema

## Runtime Flow

```text
RawPacket
  -> PacketParser
  -> FlowAggregator
  -> RuleEngine
  -> Alert queue
  -> runtime-data/*.csv
```

The API uses `database/nids.sqlite`; the C++ service currently writes CSV runtime output so it can build without extra native SQLite dependencies. A later phase can share SQLite directly from C++ or add an ingestion bridge.

## Detection Rules

Current rules are intentionally simple and explainable:

- `PORT_SCAN`: one source touches at least 25 destination ports
- `TRAFFIC_FLOOD`: a flow or source exceeds packet thresholds
- `SUSPICIOUS_PROTOCOL`: protocol is not TCP, UDP, or ICMP

## Next Phase

Add an ML service in `ml/training`:

- generate labeled feature CSV from flows
- train `IsolationForest`
- expose anomaly predictions to the detection engine
- persist threat scores with alerts

