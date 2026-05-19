import cors from 'cors';
import express from 'express';
import sqlite3 from 'sqlite3';
import { readFileSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const __dirname = dirname(fileURLToPath(import.meta.url));
const databasePath = resolve(__dirname, '../../database/nids.sqlite');
const schemaPath = resolve(__dirname, '../../database/schema.sql');
const dashboardPath = resolve(__dirname, '../../dashboard');

const db = new sqlite3.Database(databasePath);
db.exec(readFileSync(schemaPath, 'utf8'));

const app = express();
app.use(cors());
app.use(express.json());
app.use(express.static(dashboardPath));

function get(sql, params = []) {
  return new Promise((resolveQuery, reject) => {
    db.get(sql, params, (error, row) => {
      if (error) reject(error);
      else resolveQuery(row);
    });
  });
}

function all(sql, params = []) {
  return new Promise((resolveQuery, reject) => {
    db.all(sql, params, (error, rows) => {
      if (error) reject(error);
      else resolveQuery(rows);
    });
  });
}

function run(sql, params = []) {
  return new Promise((resolveQuery, reject) => {
    db.run(sql, params, function onRun(error) {
      if (error) reject(error);
      else resolveQuery(this);
    });
  });
}

app.get('/api/traffic/stats', async (_request, response, next) => {
  try {
    const latest = await get(`
      SELECT active_connections, total_packets, anomalies_detected
      FROM SystemStatistics
      ORDER BY timestamp DESC
      LIMIT 1
    `);

    response.json({
      activeConnections: latest?.active_connections ?? 0,
      packetsProcessed: latest?.total_packets ?? 0,
      anomalies: latest?.anomalies_detected ?? 0
    });
  } catch (error) {
    next(error);
  }
});

app.get('/api/alerts', async (_request, response, next) => {
  try {
    const alerts = await all(`
      SELECT alert_id AS alertId, alert_type AS alertType, severity, source_ip AS sourceIp,
             description, created_at AS createdAt
      FROM Alerts
      ORDER BY created_at DESC
      LIMIT 100
    `);
    response.json(alerts);
  } catch (error) {
    next(error);
  }
});

app.get('/api/suspicious', async (_request, response, next) => {
  try {
    const ips = await all(`
      SELECT source_ip AS ip, COUNT(*) AS alertCount, MAX(severity) AS highestSeverity
      FROM Alerts
      GROUP BY source_ip
      ORDER BY alertCount DESC
      LIMIT 25
    `);
    response.json(ips);
  } catch (error) {
    next(error);
  }
});

app.get('/api/traffic/protocols', async (_request, response, next) => {
  try {
    const protocols = await all(`
      SELECT protocol, SUM(packet_count) AS packets, SUM(bytes_transferred) AS bytes
      FROM TrafficFlow
      GROUP BY protocol
      ORDER BY packets DESC
    `);
    response.json(protocols);
  } catch (error) {
    next(error);
  }
});

app.get('/api/traffic/trends', async (_request, response, next) => {
  try {
    const trends = await all(`
      SELECT substr(created_at, 1, 16) AS timeBucket,
             SUM(packet_count) AS packets,
             SUM(bytes_transferred) AS bytes
      FROM TrafficFlow
      GROUP BY timeBucket
      ORDER BY timeBucket DESC
      LIMIT 30
    `);
    response.json(trends.reverse());
  } catch (error) {
    next(error);
  }
});

app.get('/api/traffic/top-sources', async (_request, response, next) => {
  try {
    const sources = await all(`
      SELECT source_ip AS ip, SUM(packet_count) AS packets, SUM(bytes_transferred) AS bytes
      FROM TrafficFlow
      GROUP BY source_ip
      ORDER BY packets DESC
      LIMIT 10
    `);
    response.json(sources);
  } catch (error) {
    next(error);
  }
});

app.post('/api/block', async (request, response, next) => {
  try {
    const { ip } = request.body ?? {};
    if (!ip || typeof ip !== 'string') {
      response.status(400).json({ error: 'ip is required' });
      return;
    }

    await run(
      'INSERT OR IGNORE INTO BlockedIPs (ip, created_at) VALUES (?, CURRENT_TIMESTAMP)',
      [ip]
    );

    response.status(201).json({ ip, status: 'blocked' });
  } catch (error) {
    next(error);
  }
});

app.get('/', (_request, response) => {
  response.sendFile(resolve(dashboardPath, 'index.html'));
});

app.use((error, _request, response, _next) => {
  console.error(error);
  response.status(500).json({ error: 'internal_server_error' });
});

const port = Number(process.env.PORT ?? 3000);
app.listen(port, () => {
  console.log(`NIDS API listening on http://localhost:${port}`);
});
