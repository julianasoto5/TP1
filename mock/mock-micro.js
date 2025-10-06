// mock-micro.js
const express = require('express');
const bodyParser = require('body-parser');

const app = express();
app.use(bodyParser.json());

let state = {
  score: 0,
  time_left: 60,
  state: 'idle', // idle, running, finished
  level: 1,
  device: 'mesa1',
  ts: new Date().toISOString()
};

// Endpoint que usa tu server: GET /status
app.get('/status', (req, res) => {
  res.json(state);
});

// Endpoint que usa tu server: POST /command
app.post('/command', (req, res) => {
  const { command, level } = req.body || {};
  if (command === 'START') {
    state.state = 'running';
    state.time_left = 60;
    state.score = 0;
    if (level) state.level = level;
  } else if (command === 'RESET') {
    state.state = 'idle';
    state.time_left = 60;
    state.score = 0;
  } else if (command === 'SELECT_LEVEL') {
    if (level) state.level = level;
  }
  state.ts = new Date().toISOString();
  res.json({ ok: true, state });
});

// Endpoint para actualizar manualmente el estado mientras probás
// POST /update-status { score, time_left, state, level }
app.post('/update-status', (req, res) => {
  const body = req.body || {};
  if (typeof body.score === 'number') state.score = body.score;
  if (typeof body.time_left === 'number') state.time_left = body.time_left;
  if (typeof body.state === 'string') state.state = body.state;
  if (typeof body.level === 'number') state.level = body.level;
  state.ts = new Date().toISOString();
  res.json({ ok: true, state });
});

// Optional: decrement time every second when running (nice to see)
setInterval(() => {
  if (state.state === 'running' && state.time_left > 0) {
    state.time_left -= 1;
    if (state.time_left <= 0) {
      state.time_left = 0;
      state.state = 'finished';
    }
    state.ts = new Date().toISOString();
  }
}, 1000);

const PORT = 5000;
app.listen(PORT, () => console.log(`Mock microcontroller running on http://localhost:${PORT}`));
