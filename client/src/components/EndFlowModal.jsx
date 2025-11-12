// client/src/components/EndFlowModal.jsx
import React, { useContext, useEffect, useState } from 'react';
import { GameContext } from '../context/GameContext';

export default function EndFlowModal({ open }) {
  const { lastScore, checkIfTop, saveScore, resetGame } = useContext(GameContext);
  const [isTop, setIsTop] = useState(false);
  const [name, setName] = useState('');
  const [saving, setSaving] = useState(false);

  useEffect(() => {
    if (!open) return;
    (async () => {
      const top = await checkIfTop(lastScore ?? 0);
      setIsTop(top);
    })();
  }, [open, lastScore, checkIfTop]);

  if (!open) return null;

  async function submit() {
    if (!name) return;
    setSaving(true);
    const ok = await saveScore(name, lastScore);
    setSaving(false);
    if (ok) resetGame();
  }

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/50">
      <div className="bg-white rounded-xl w-full max-w-lg p-6 shadow-lg" onClick={(e) => e.stopPropagation()}>
        <h2 className="text-xl font-bold text-slate-900">Fin del tiempo</h2>

        <div className="mt-4 text-slate-700">
          <p>Tu puntaje: <span className="font-bold text-slate-900">{lastScore}</span></p>
        </div>

        {isTop ? (
          <div className="mt-4">
            <p className="text-slate-600">Ingresá tu nombre para guardar en el Top 5</p>
            <input
              className="mt-3 w-full px-3 py-2 border border-slate-200 rounded-md"
              value={name}
              onChange={(e) => setName(e.target.value)}
              placeholder="Tu nombre"
            />
            <div className="mt-4 flex justify-end gap-3">
              <button className="px-4 py-2 rounded-md border" onClick={() => resetGame()}>Cancelar</button>
              <button className="px-4 py-2 rounded-md bg-blue-600 text-white font-semibold disabled:opacity-50" disabled={!name || saving} onClick={submit}> {saving ? 'Guardando...' : 'Guardar'} </button>
            </div>
          </div>
        ) : (
          <div className="mt-4 flex justify-end">
            <button className="px-4 py-2 rounded-md bg-blue-600 text-white font-semibold" onClick={() => resetGame()}> Volver al inicio </button>
          </div>
        )}
      </div>
    </div>
  );
}
