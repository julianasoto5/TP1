// client/src/context/GameContext.jsx
import React, { createContext, useState, useEffect, useRef, useCallback } from 'react';
import axios from 'axios';

const API = import.meta.env.VITE_API_URL || 'http://localhost:4000/api';

export const GameContext = createContext();

export function GameProvider({ children }) {
  const [phase, setPhase] = useState('welcome'); // welcome, selecting, running, ended
  const [selectedLevel, setSelectedLevel] = useState(null); // 1 or 3
  const [status, setStatus] = useState({ score: 0, time_left: 0, state: 'idle' });
  const [lastScore, setLastScore] = useState(null);
  const [ranking, setRanking] = useState({ '1': [], '3': [] }); // store per-level
  const [connected, setConnected] = useState(true);
  const pollingRef = useRef(null);

  // stopPolling must be stable (no recreations that break intervals)
  const stopPolling = useCallback(() => {
    if (pollingRef.current) {
      clearInterval(pollingRef.current);
      pollingRef.current = null;
    }
  }, []);

  // load top5 for a given level (1 or 3) — memoized so reference is stable
  const loadRanking = useCallback(async function loadRanking(level = 1) {
    try {
      const res = await axios.get(`${API}/ranking`, { params: { level } });
      setRanking(prev => ({ ...prev, [String(level)]: res.data || [] }));
    } catch (e) {
      console.error('loadRanking', e);
      setRanking(prev => ({ ...prev, [String(level)]: [] }));
    }
  }, []);

  // load both levels (useful at startup) — depends on the stable loadRanking
  const loadBothRankings = useCallback(async function loadBothRankings() {
    await Promise.all([loadRanking(1), loadRanking(3)]);
  }, [loadRanking]);

  useEffect(() => {
    // al montar, cargamos ambos top5 para mostrar en el modal de ranking
    loadBothRankings();
    return () => stopPolling();
  }, [loadBothRankings, stopPolling]);

  async function sendCommand(command, level = undefined) {
    try {
      await axios.post(`${API}/command`, { command, level });
      return true;
    } catch (e) {
      console.error('sendCommand', e);
      return false;
    }
  }

  function startPolling(intervalMs = 700) {
    stopPolling();
    pollingRef.current = setInterval(async () => {
      try {
        const res = await axios.get(`${API}/status`);
        setStatus(res.data || { score: 0, time_left: 0, state: 'idle' });
        setConnected(true);
        if ((res.data?.time_left ?? 0) <= 0 || res.data?.state === 'finished') {
          stopPolling();
          setLastScore(res.data?.score ?? 0);
          setPhase('ended');
        }
      } catch (e) {
        console.error('poll error', e);
        setConnected(false);
      }
    }, intervalMs);
  }

  async function startGame(level) {
    setSelectedLevel(level);
    await sendCommand('SELECT_LEVEL', level);
    setPhase('running');
    await sendCommand('START', level);
    startPolling();
  }

  async function resetGame() {
    stopPolling();
    await sendCommand('RESET');
    setSelectedLevel(null);
    setStatus({ score: 0, time_left: 0, state: 'idle' });
    setLastScore(null);
    setPhase('welcome');
  }

  // saveScore uses selectedLevel; if not set, defaults to 1
  async function saveScore(name, score) {
    const level = Number(selectedLevel ?? 1);
    try {
      await axios.post(`${API}/ranking`, { name, score, level });
      // actualizar ranking del nivel correspondiente
      await loadRanking(level);
      return true;
    } catch (e) {
      console.error('saveScore', e);
      return false;
    }
  }

  // checkIfTop queries the server-side endpoint for the currently selected level
  async function checkIfTop(score) {
    const level = Number(selectedLevel ?? 1);
    try {
      const res = await axios.get(`${API}/ranking/check`, { params: { level, score } });
      // espera { isTop: bool, worstScore: number|null }
      return res.data?.isTop ?? true;
    } catch (e) {
      console.error('checkIfTop', e);
      // en caso de error asumimos que sí entra para no bloquear la UX
      return true;
    }
  }

  return (
    <GameContext.Provider
      value={{
        phase,
        setPhase,
        selectedLevel,
        setSelectedLevel,
        status,
        lastScore,
        ranking,
        connected,
        startGame,
        resetGame,
        loadRanking,
        loadBothRankings,
        saveScore,
        checkIfTop,
        sendCommand
      }}
    >
      {children}
    </GameContext.Provider>
  );
}
