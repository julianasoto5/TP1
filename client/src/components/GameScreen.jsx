// client/src/components/GameScreen.jsx
import React, { useContext, useEffect } from 'react';
import { GameContext } from '../context/GameContext';

export default function GameScreen() {
  const { status, resetGame, selectedLevel, connected } = useContext(GameContext);

  const timeBig = status?.time_left;
  const score = status?.score;
  const warn = timeBig <= 10;

  return (
    <div className="h-[70vh] flex flex-col">
      <div className="flex items-center justify-between px-4 py-2">
        <div className={`px-3 py-1 rounded-md font-semibold ${connected ? 'bg-emerald-100 text-emerald-800' : 'bg-rose-100 text-rose-700'}`}> {connected ? 'Online' : 'Offline'} </div>
        <div className="text-sm text-slate-600">Nivel: <span className="font-semibold text-slate-800">{selectedLevel ?? '-'}</span></div>
        <button className="px-3 py-1 rounded-md border border-slate-200 text-slate-700 hover:bg-slate-50" onClick={resetGame}> Salir </button>
      </div>

      <div className="flex-1 flex flex-col items-center justify-center gap-6">
        <div className={`rounded-lg px-6 py-4 text-center ${warn ? 'text-rose-600 animate-pulse' : 'text-slate-900'}`}>
          <div className={`text-[8rem] leading-none font-extrabold select-none ${warn ? 'scale-[1.03] transition-transform' : ''}`}>
            {timeBig}s
          </div>
        </div>

        <div className="text-2xl text-slate-800"> Puntaje: <span className="font-bold text-3xl">{score}</span> </div>
      </div>
    </div>
  );
}
