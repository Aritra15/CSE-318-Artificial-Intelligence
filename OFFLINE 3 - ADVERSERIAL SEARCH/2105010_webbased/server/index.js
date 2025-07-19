const express = require('express');
const cors    = require('cors');
const {
  createBoard,
  applyMove,
  isGameOver,
  minimaxSearch
} = require('./game');

const app = express();
app.use(cors());
app.use(express.json());

let board         = createBoard();
let currentPlayer = 'R';
let humanPlayed   = false;
let aiPlayed      = false;
let winner        = null;

// Convert typed‐array board to array-of-arrays for front end
function serializeBoard(b) {
  const out = [];
  for (let r = 0; r < 9; r++) {
    const row = [];
    for (let c = 0; c < 6; c++) {
      const idx = r * 6 + c;
      const cnt = b.counts[idx];
      const col = b.colors[idx] === 1 ? 'R'
                : b.colors[idx] === 2 ? 'B'
                : ' ';
      row.push({ count: cnt, color: col });
    }
    out.push(row);
  }
  return out;
}

app.get('/api/state', (req, res) => {
  res.json({
    board: serializeBoard(board),
    currentPlayer,
    humanPlayed,
    aiPlayed,
    winner
  });
});

// Human move
app.post('/api/move', (req, res) => {
  const { row, col } = req.body;
  console.log(`Human move: ${row}, ${col}`);

  try {
    applyMove(board, row, col, 1); 
    humanPlayed = true;

    const rawWinner = isGameOver(board);
    if (rawWinner && humanPlayed && aiPlayed) {
      winner = rawWinner;
      return res.json({ board: serializeBoard(board), winner });
    }

    currentPlayer = 'B';
    return res.json({ board: serializeBoard(board), currentPlayer });
  } catch (e) {
    return res.status(400).json({ error: e.message });
  }
});

// AI move (minimax)
app.post('/api/ai-move', (req, res) => {
  if (winner) {
    return res.json({ board: serializeBoard(board), winner });
  }

  const { r, c } = minimaxSearch(board);
  console.log(`AI move (minimax): ${r}, ${c}`);
  applyMove(board, r, c, 2);    // 2 = AI (B)
  aiPlayed = true;

  const rawWinner = isGameOver(board);
  if (rawWinner && humanPlayed && aiPlayed) {
    winner = rawWinner;
    return res.json({ board: serializeBoard(board), winner });
  }

  currentPlayer = 'R';
  return res.json({ board: serializeBoard(board), currentPlayer });
});

app.post('/api/reset', (req, res) => {
  board         = createBoard();
  currentPlayer = 'R';
  humanPlayed   = false;
  aiPlayed      = false;
  winner        = null;
  res.json({ board: serializeBoard(board), currentPlayer });
});

app.listen(4000, () => console.log('Server running on http://localhost:4000'));
