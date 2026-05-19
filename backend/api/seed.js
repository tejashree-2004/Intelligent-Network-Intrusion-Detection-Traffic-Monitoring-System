import sqlite3 from 'sqlite3';
import { readFileSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const __dirname = dirname(fileURLToPath(import.meta.url));
const databasePath = resolve(__dirname, '../../database/nids.sqlite');
const schemaPath = resolve(__dirname, '../../database/schema.sql');
const db = new sqlite3.Database(databasePath);

function run(sql, params = []) {
  return new Promise((resolveQuery, reject) => {
    db.run(sql, params, function onRun(error) {
      if (error) reject(error);
      else resolveQuery(this);
    });
  });
}

db.exec(readFileSync(schemaPath, 'utf8'));

const protocols = ['TCP', 'UDP', 'ICMP'];
const suspiciousIp = '192.168.1.50';

for (let index = 0; index < 60; index += 1) {
  const source = index % 9 === 0 ? suspiciousIp : `192.168.1.${10 + (index % 30)}`;
  const protocol = protocols[index % protocols.length];
  const packetCount = index % 9 === 0 ? 240 + index : 20 + (index % 40);
  const bytes = packetCount * (180 + (index % 12) * 35);
  const destinationPort = index % 9 === 0 ? 20 + index : 443;

  await run(
    `INSERT INTO TrafficFlow
      (source_ip, destination_ip, source_port, destination_port, protocol,
       packet_count, bytes_transferred, duration, created_at)
     VALUES (?, ?, ?, ?, ?, ?, ?, ?, datetime('now', ?))`,
    [
      source,
      `10.0.0.${2 + (index % 12)}`,
      30000 + index,
      destinationPort,
      protocol,
      packetCount,
      bytes,
      Number((0.2 + index / 20).toFixed(2)),
      `-${60 - index} minutes`
    ]
  );
}

await run(
  `INSERT INTO Alerts (alert_type, severity, source_ip, description, created_at)
   VALUES (?, ?, ?, ?, CURRENT_TIMESTAMP)`,
  [
    'TRAFFIC_FLOOD',
    'CRITICAL',
    suspiciousIp,
    'Traffic frequency exceeded threshold by 320%. Detected behavior resembles DDoS traffic.'
  ]
);

await run(
  `INSERT INTO Alerts (alert_type, severity, source_ip, description, created_at)
   VALUES (?, ?, ?, ?, datetime('now', '-4 minutes'))`,
  [
    'PORT_SCAN',
    'HIGH',
    suspiciousIp,
    'Single source contacted many destination ports in the monitoring window.'
  ]
);

await run(
  `INSERT INTO SystemStatistics
    (active_connections, total_packets, anomalies_detected, cpu_usage, timestamp)
   VALUES (?, ?, ?, ?, CURRENT_TIMESTAMP)`,
  [48, 35000, 2, 18.4]
);

db.close();
console.log('Seeded demo NIDS data.');

