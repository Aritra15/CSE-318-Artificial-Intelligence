import React, { useEffect, useState } from 'react';
import axios from 'axios';
import GameBoard from './components/GameBoard.jsx';

export default function App() {
  const [state, setState] = useState({
    board: [],
    currentPlayer: 'R',
    winner: null
  });

  // Fetch the latest board & turn
  const fetchState = async () => {
    try {
      const res = await axios.get('http://localhost:4000/api/state');
      setState(res.data);
    } catch (e) {
      console.error('Failed to fetch state:', e);
    }
  };

  useEffect(() => {
    fetchState();
  }, []);

  useEffect(() => {
  if (state.winner) {
    alert(`Game Over!\nWinner: ${state.winner === 'R' ? 'Human (Red)' : 'AI (Blue)'}`);
  }
}, [state.winner]);


  const handleCellClick = async (r, c) => {
    // block clicks if game over or not human's turn
    if (state.winner || state.currentPlayer !== 'R') return;

    try {
      // 1) Human move
      console.log(`▶ Sending human move: ${r}, ${c}`);
      const humanRes = await axios.post('http://localhost:4000/api/move', {
        row: r,
        col: c
      });
      setState(humanRes.data);

      // 2) AI move (only if no winner yet)
      if (!humanRes.data.winner) {
        console.log('▶ Sending AI move');
        const aiRes = await axios.post('http://localhost:4000/api/ai-move');
        setState(aiRes.data);
      }

    } catch (err) {
      // Log backend error message
      console.error('Move failed:', err.response?.data?.error || err.message);
    }
  };

  const handleReset = async () => {
    try {
      await axios.post('http://localhost:4000/api/reset');
      fetchState();
    } catch (e) {
      console.error('Reset failed:', e);
    }
  };

  return (
    <div className="app-container">
      <h1>Chain Reaction</h1>
      <button onClick={handleReset} className='btn btn-primary' style={{ marginBottom: '1rem' }}>Restart</button>
      <GameBoard board={state.board} onCellClick={handleCellClick}  />
      {state.winner && (
  <div className={`alert ${state.winner === 'R' ? 'alert-danger' : 'alert-primary'} mt-3`} role="alert" style={{marginTop: '1rem'}}>
     <strong>Winner:</strong> {state.winner === 'R' ? 'Human ( Red)' : 'AI ( Blue)'}
  </div>
)}

    </div>
  );
}
