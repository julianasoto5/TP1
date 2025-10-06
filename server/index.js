// index.js
require('dotenv').config();
const express = require('express');
const cors = require('cors');
const path = require('path');
const fetch = global.fetch || require('node-fetch'); 
const { getTop, addScore, isTopForLevel, DB_FILE } = require('./storage');

const app = express();
app.use(cors());
app.use(express.json());

const PORT = process.env.PORT || 4000;
const MICRO_URL = (process.env.MICRO_URL || '').replace(/\/+$/, '');
const TOP_N = 5;

// Helper: proxy to microcontroller or mock
async function callMicro(endpoint, options = {}) {
  if (!MICRO_URL) {
    if (endpoint === '/status') {
      return { score: Math.floor(Math.random() * 1000), time_left: 60, state: 'idle' };
    }
    if (endpoint === '/command') {
      return { ok: true };
    }
    return {};
  }
  const url = `${MICRO_URL}${endpoint}`;
  const res = await fetch(url, options);
  if (!res.ok) throw new Error(`Microcontroller responded ${res.status}`);
  return res.json();
}

// API: enviar comando START, RESET, SELECT_LEVEL
app.post('/api/command', async (req, res) => {
  const { command, level } = req.body;
  if (!command) return res.status(400).json({ error: 'command required' });
  try {
    const resp = await callMicro('/command', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ command, level })
    });
    return res.json({ result: resp });
  } catch (err) {
    return res.status(500).json({ error: err.message });
  }
});

// API: obtener estado en tiempo real
app.get('/api/status', async (req, res) => {
  try {
    const status = await callMicro('/status');
    return res.json(status);
  } catch (err) {
    console.error('ERROR /api/status', err && err.stack ? err.stack : err);
    res.status(500).json({ error: 'internal server error' });
  }
});

// Ranking: GET top 5 por nivel
app.get('/api/ranking', async (req, res) => {
  try {
    const level = Number(req.query.level ?? 1);
    if (![1, 3].includes(level)) return res.status(400).json({ error: 'level must be 1 or 3' });
    const top = await getTop(level, TOP_N);
    res.json(top);
  } catch (err) {
    console.error('ERROR /api/ranking', err && err.stack ? err.stack : err);
    res.status(500).json({ error: 'internal server error' });
  }
});

// Ranking: POST nuevo puntaje (requiere name, score, level)
app.post('/api/ranking', async (req, res) => {
  const { name, score, level } = req.body;
  if (!name || typeof score !== 'number' || ![1, 3].includes(Number(level))) {
    return res.status(400).json({ error: 'requiere name(string), score(number), level(1|3)' });
  }
  try {
    const updated = await addScore(String(name).trim().slice(0, 30), Number(score), Number(level));
    return res.json({ ok: true, ranking: updated });
  } catch (err) {
    console.error('ERROR POST /api/ranking', err && err.stack ? err.stack : err);
    return res.status(500).json({ error: err.message });
  }
});

// Ranking: chequea si entra
app.get('/api/ranking/check', async (req, res) => {
  try {
    const level = Number(req.query.level ?? 1);
    const score = Number(req.query.score);
    if (![1, 3].includes(level) || Number.isNaN(score)) {
      return res.status(400).json({ error: 'parametros invalidos' });
    }
    const result = await isTopForLevel(level, score, TOP_N);
    res.json(result); 
  } catch (err) {
    console.error('ERROR /api/ranking/check', err && err.stack ? err.stack : err);
    res.status(500).json({ error: err.message });
  }
});

// Endpoint para comprobar archivo de datos
app.get('/api/dbfile', (req, res) => {
  res.json({ path: DB_FILE });
});

app.listen(PORT, () => console.log(`Server listening on ${PORT}`));
