// client/src/components/WelcomeScreen.jsx
import React, { useEffect } from 'react';

export default function WelcomeScreen({ onOpenLevel, onOpenRanking }) {
  useEffect(() => {
    document.body.classList.add('no-scroll');
    return () => document.body.classList.remove('no-scroll');
  }, []);

  return (
    <div className="welcome-full relative">
      <div className="absolute inset-0 bg-gradient-to-br from-blue-50 via-sky-50 to-cyan-50"></div>
      <div className="relative z-10 max-w-2xl w-full p-8 bg-white/90 rounded-2xl shadow-xl text-center mx-4">
        <h1 className="text-3xl font-extrabold text-slate-900">Bienvenido</h1>
        <p className="mt-2 text-slate-600">Prepará tus reflejos. Jugá contra el tiempo.</p>
        <div className="mt-8 flex flex-col gap-3 sm:flex-row sm:justify-center">
          <button className="w-full sm:w-auto px-6 py-3 rounded-lg bg-blue-600 text-white" onClick={onOpenLevel}>Comenzar</button>
          <button className="w-full sm:w-auto px-6 py-3 rounded-lg border" onClick={onOpenRanking}>Historial</button>
          
        </div>
      </div>
    </div>
  );
}
