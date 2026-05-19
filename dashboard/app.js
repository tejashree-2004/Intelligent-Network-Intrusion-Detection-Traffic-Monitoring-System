const apiBase = window.location.port === '5173' ? 'http://localhost:3000' : '';

const numberFormat = new Intl.NumberFormat('en-US');

async function request(path) {
  const response = await fetch(`${apiBase}${path}`);
  if (!response.ok) {
    throw new Error(`Request failed: ${path}`);
  }
  return response.json();
}

function setText(id, value) {
  document.getElementById(id).textContent = numberFormat.format(value ?? 0);
}

function renderTrend(rows) {
  const chart = document.getElementById('trendChart');
  const max = Math.max(...rows.map((row) => row.packets), 1);
  chart.innerHTML = rows
    .map((row) => {
      const height = Math.max(4, Math.round((row.packets / max) * 100));
      return `<div class="bar" title="${row.timeBucket}: ${row.packets} packets" style="height:${height}%"></div>`;
    })
    .join('');
}

function renderList(id, rows, formatter) {
  const element = document.getElementById(id);
  element.innerHTML = rows.length
    ? rows.map(formatter).join('')
    : '<div class="item"><span>No data yet</span><strong>0</strong></div>';
}

function renderAlerts(alerts) {
  const element = document.getElementById('alertsList');
  element.innerHTML = alerts.length
    ? alerts
        .map(
          (alert) => `
            <div class="alert">
              <strong>${alert.alertType}</strong>
              <b>${alert.severity}</b>
              <p>${alert.sourceIp}: ${alert.description}</p>
            </div>
          `
        )
        .join('')
    : '<div class="alert"><strong>No alerts</strong><p>Monitoring is quiet.</p></div>';
}

async function refresh() {
  const [stats, alerts, suspicious, protocols, trends, topSources] = await Promise.all([
    request('/api/traffic/stats'),
    request('/api/alerts'),
    request('/api/suspicious'),
    request('/api/traffic/protocols'),
    request('/api/traffic/trends'),
    request('/api/traffic/top-sources')
  ]);

  setText('activeConnections', stats.activeConnections);
  setText('packetsProcessed', stats.packetsProcessed);
  setText('anomalies', stats.anomalies);
  setText('packetRate', trends.at(-1)?.packets ?? 0);

  renderTrend(trends);
  renderAlerts(alerts);
  renderList(
    'protocolList',
    protocols,
    (row) => `<div class="item"><span>${row.protocol}</span><strong>${numberFormat.format(row.packets)}</strong></div>`
  );
  renderList(
    'suspiciousList',
    suspicious,
    (row) => `<div class="item"><span>${row.ip}</span><strong>${row.alertCount}</strong></div>`
  );
  renderList(
    'topSources',
    topSources,
    (row) => `<div class="item"><span>${row.ip}</span><strong>${numberFormat.format(row.packets)}</strong></div>`
  );
}

document.getElementById('refreshButton').addEventListener('click', refresh);
refresh().catch((error) => {
  console.error(error);
});
