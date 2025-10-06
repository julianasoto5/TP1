// client/src/components/LevelSelect.jsx
import React, { useState, useEffect } from 'react';

export default function LevelSelect({ open, onClose, onConfirm }) {
  const [level, setLevel] = useState(null);

  useEffect(() => {
    if (!open) setLevel(null);
  }, [open]);

  if (!open) return null;
  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/50" onClick={onClose}>
      <div className="bg-white rounded-xl w-full max-w-md p-6 shadow-lg" onClick={(e) => e.stopPropagation()}>
        <h2 className="text-xl font-bold text-slate-900">Seleccione el nivel para comenzar</h2>

        <div className="mt-6 flex gap-4 justify-center">
          <button className={`px-5 py-3 rounded-lg font-semibold ${level === 1 ? 'bg-blue-600 text-white' : 'bg-white border border-slate-200 text-slate-800'}`} onClick={() => setLevel(1)}>FÁCIL</button>
          <button className={`px-5 py-3 rounded-lg font-semibold ${level === 3 ? 'bg-rose-600 text-white' : 'bg-white border border-slate-200 text-slate-800'}`} onClick={() => setLevel(3)}>DIFÍCIL</button>
        </div>

        <div className="mt-6 flex justify-end gap-3">
          <button className="px-4 py-2 rounded-md border border-slate-200 text-slate-700" onClick={onClose}>Cancelar</button>
          <button className="px-4 py-2 rounded-md bg-blue-600 text-white font-semibold disabled:opacity-50" disabled={!level} onClick={() => { onConfirm(level); }}>Iniciar</button>
        </div>
      </div>
    </div>
  );
}
