CREATE TABLE IF NOT EXISTS TrafficFlow (
    flow_id INTEGER PRIMARY KEY AUTOINCREMENT,
    source_ip TEXT NOT NULL,
    destination_ip TEXT NOT NULL,
    source_port INTEGER NOT NULL,
    destination_port INTEGER NOT NULL,
    protocol TEXT NOT NULL,
    packet_count INTEGER NOT NULL,
    bytes_transferred INTEGER NOT NULL,
    duration REAL NOT NULL,
    created_at TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS Alerts (
    alert_id INTEGER PRIMARY KEY AUTOINCREMENT,
    alert_type TEXT NOT NULL,
    severity TEXT NOT NULL,
    source_ip TEXT NOT NULL,
    description TEXT NOT NULL,
    created_at TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS SystemStatistics (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    active_connections INTEGER NOT NULL,
    total_packets INTEGER NOT NULL,
    anomalies_detected INTEGER NOT NULL,
    cpu_usage REAL NOT NULL DEFAULT 0,
    timestamp TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS BlockedIPs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ip TEXT NOT NULL UNIQUE,
    created_at TEXT NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_flows_created_at ON TrafficFlow(created_at);
CREATE INDEX IF NOT EXISTS idx_alerts_created_at ON Alerts(created_at);
CREATE INDEX IF NOT EXISTS idx_alerts_source_ip ON Alerts(source_ip);

