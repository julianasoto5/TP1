// client/src/App.jsx
import React, { useContext, useState, useEffect } from 'react';
import { GameProvider, GameContext } from './context/GameContext';
import WelcomeScreen from './components/WelcomeScreen';
import LevelSelect from './components/LevelSelect';
import GameScreen from './components/GameScreen';
import EndFlowModal from './components/EndFlowModal';
import RankingModal from './components/RankingModal';
import './index.css'; // Tailwind

function AppInner() {
  const { phase, startGame } = useContext(GameContext);
  const [levelModalOpen, setLevelModalOpen] = useState(false);
  const [rankingOpen, setRankingOpen] = useState(false);

  useEffect(() => {
    const onOpen = () => setRankingOpen(true);
    window.addEventListener('open-ranking', onOpen);
    return () => window.removeEventListener('open-ranking', onOpen);
  }, []);

  return (
    <div className="min-h-screen bg-slate-50">
      {phase === 'welcome' && (
        <WelcomeScreen
          onOpenLevel={() => setLevelModalOpen(true)}
          onOpenRanking={() => setRankingOpen(true)}
        />
        
      )}
      

      {phase === 'running' && (
        <div className="p-6">
          <GameScreen />
        </div>
      )}

      {phase === 'ended' && (
        <div className="p-6">
          <GameScreen />
        </div>
      )}

      <LevelSelect open={levelModalOpen} onClose={() => setLevelModalOpen(false)} onConfirm={async (lvl) => { setLevelModalOpen(false); await startGame(lvl); }} />

      <EndFlowModal open={phase === 'ended'} />

      <RankingModal open={rankingOpen} onClose={() => setRankingOpen(false)} />
    </div>
  );
}

export default function App() {
  return (
    <GameProvider>
      <AppInner />
    </GameProvider>
  );
}
