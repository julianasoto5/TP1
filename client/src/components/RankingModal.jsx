// client/src/components/RankingModal.jsx
import React, { useContext, useEffect, useState } from 'react';
import { GameContext } from '../context/GameContext';

export default function RankingModal({ open, onClose }) {
  // hooks: deben ejecutarse siempre en el mismo orden
  const { ranking, loadRanking, loadBothRankings } = useContext(GameContext);
  const [tab, setTab] = useState(1);
  const [loading, setLoading] = useState(false);

  // cargar al abrir y resetear tab (dependencia estable)
  useEffect(() => {
    if (!open) return;
    setLoading(true);
    loadBothRankings().finally(() => setLoading(false));
    setTab(1);
  }, [open, loadBothRankings]);

  // cargar al cambiar de pestaña
  useEffect(() => {
    if (!open) return;
    let cancelled = false;
    (async () => {
      setLoading(true);
      await loadRanking(tab);
      if (!cancelled) setLoading(false);
    })();
    return () => { cancelled = true; };
  }, [tab, open, loadRanking]);


  if (!open) return null;

  const listFor = (level) => (ranking && ranking[String(level)] ? ranking[String(level)] : []);

  const renderList = (items) => {
    if (loading) return <div className="text-slate-600">Cargando...</div>;
    if (!items || items.length === 0) return <div className="text-slate-600">No hay registros</div>;
    return (
      <ol className="mt-2 space-y-2">
        {items.map((r, i) => (
          <li key={i} className="flex items-center gap-4 p-3 rounded-md bg-slate-50">
            <div className="w-10 h-10 rounded-md bg-sky-400 text-white flex items-center justify-center font-bold">{i + 1}</div>
            <div>
              <div className="font-semibold text-slate-800">{r.name}</div>
              <div className="text-sm text-slate-500">{r.score} pts ·{r.date ? new Date(r.date).toLocaleString() : 'sin fecha'}</div>
            </div>
          </li>
        ))}
      </ol>
    );
  };

  const safeClose = () => {
    if (typeof onClose === 'function') onClose();
  };

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/50" onClick={safeClose}>
      <div className="bg-white rounded-xl w-full max-w-2xl p-6 shadow-lg" onClick={(e) => e.stopPropagation()} role="dialog" aria-modal="true" aria-labelledby="ranking-title">
        <div className="flex items-center justify-between">
          <h2 id="ranking-title" className="text-lg font-bold text-slate-900">Top 5 - Ranking</h2>

          <div className="flex gap-2">
            <button className="px-3 py-1 rounded-md border" onClick={() => { setLoading(true); loadRanking(tab).finally(() => setLoading(false)); }}> Actualizar </button>
            <button className="px-3 py-1 rounded-md border" onClick={safeClose}>Cerrar</button>
          </div>
        </div>

        <div className="mt-4">
          <div className="flex gap-2 border-b pb-3">
            <button className={`px-4 py-2 rounded-t-md ${tab === 1 ? 'bg-white border border-b-0 -mb-px font-semibold' : 'bg-transparent text-slate-600'}`} onClick={() => setTab(1)}> FÁCIL </button>
            <button className={`px-4 py-2 rounded-t-md ${tab === 3 ? 'bg-white border border-b-0 -mb-px font-semibold' : 'bg-transparent text-slate-600'}`} onClick={() => setTab(3)}> DIFÍCIL </button>
          </div>

          <div className="mt-4">
            {tab === 1 && renderList(listFor(1))}
            {tab === 3 && renderList(listFor(3))}
          </div>
        </div>
      </div>
    </div>
  );
}
