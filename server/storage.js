// storage.js
const fs = require('fs/promises');
const path = require('path');

const DB_FILE = path.join(__dirname, 'ranking.json');
const TEMP_FILE = path.join(__dirname, 'ranking.json.tmp');
const DEFAULT_TOP_N = 5;

let mutex = Promise.resolve();

async function _withLock(fn) {
  const prev = mutex;
  let release;
  mutex = new Promise(resolve => (release = resolve));
  await prev;
  try {
    return await fn();
  } finally {
    release();
  }
}

async function _readRaw() {
  try {
    const raw = await fs.readFile(DB_FILE, 'utf8');
    return JSON.parse(raw);
  } catch (err) {
    if (err.code === 'ENOENT') {
      return { rankings: { '1': [], '3': [] } };
    }
    // Si JSON corrupto, renueva archivo vacío en lugar de romper
    if (err instanceof SyntaxError) {
      console.error('storage._readRaw: JSON corrupto — regenerando', err.message);
      return { rankings: { '1': [], '3': [] } };
    }
    throw err;
  }
}

async function _writeAtomic(obj) {
  const data = JSON.stringify(obj, null, 2);
  await fs.writeFile(TEMP_FILE, data, 'utf8');
  await fs.rename(TEMP_FILE, DB_FILE);
}

function _normalizeDb(db) {
  if (db && Array.isArray(db.ranking)) {
    const old = db.ranking.map(r => ({
      name: r.name,
      score: r.score,
      date: r.date || new Date().toISOString(),
      level: r.level || 1
    }));
    return { rankings: { '1': old, '3': [] } };
  }
  if (!db || typeof db !== 'object') return { rankings: { '1': [], '3': [] } };
  if (!db.rankings) return { rankings: { '1': [], '3': [] } };
  if (!Array.isArray(db.rankings['1'])) db.rankings['1'] = [];
  if (!Array.isArray(db.rankings['3'])) db.rankings['3'] = [];
  return db;
}

function _sortDesc(a, b) {
  if (b.score !== a.score) return b.score - a.score;
  return new Date(a.date) - new Date(b.date);
}

// getTop(level, n)
async function getTop(level, n = DEFAULT_TOP_N) {
  return _withLock(async () => {
    const raw = await _readRaw();
    const db = _normalizeDb(raw);
    const lvl = String(level);
    const arr = (db.rankings[lvl] || []).slice().sort(_sortDesc);
    return arr.slice(0, n);
  }); 
}

// addScore(name, score, level)
async function addScore(name, score, level) {
  if (typeof score !== 'number' || !name) {
    throw new Error('name and numeric score required');
  }
  return _withLock(async () => {
    const raw = await _readRaw();
    const db = _normalizeDb(raw);
    const lvl = String(level);
    if (!db.rankings[lvl]) db.rankings[lvl] = [];
    const entry = { name, score, date: new Date().toISOString(), level: Number(level) };
    db.rankings[lvl].push(entry);
    db.rankings[lvl] = db.rankings[lvl].slice().sort(_sortDesc).slice(0, DEFAULT_TOP_N);
    await _writeAtomic(db);
    return db.rankings[lvl].slice();
  });
}

// isTopForLevel(level, score, n)
async function isTopForLevel(level = 1, score = 0, n = DEFAULT_TOP_N) {
  return _withLock(async () => {
    const raw = await _readRaw();
    const db = _normalizeDb(raw);
    const lvl = String(level);
    const arr = (db.rankings[lvl] || []).slice().sort(_sortDesc);
    if (arr.length < n) return { isTop: true, worstScore: null };
    const worst = arr[arr.length - 1].score;
    return { isTop: score > worst, worstScore: worst };
  });
}

module.exports = { getTop, addScore, isTopForLevel, DB_FILE: DB_FILE };
