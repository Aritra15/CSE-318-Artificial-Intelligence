const ROWS      = 9;
const COLS      = 6;
const CELLS     = ROWS * COLS;
const MAX_DEPTH = 4;

function createBoard() {
  return {
    counts: new Uint8Array(CELLS),
    colors: new Uint8Array(CELLS),
  };
}

function index(r, c) {
  return r * COLS + c;
}

function getCriticalMass(r, c) {
  const corner = (r === 0 || r === ROWS - 1) && (c === 0 || c === COLS - 1);
  const edge   = (r === 0 || r === ROWS - 1) || (c === 0 || c === COLS - 1);
  if (corner) return 2;
  if (edge)   return 3;
  return 4;
}

function applyMove(board, r, c, playerColor) {
  const i = index(r, c);
  if (board.colors[i] === 0 || board.colors[i] === playerColor) {
    board.counts[i] += 1;
    board.colors[i]  = playerColor;
  } else {
    throw new Error('Invalid move');
  }

  const queue = [i];
  while (queue.length) {
    const idx = queue.shift();
    const rr  = Math.floor(idx / COLS);
    const cc  = idx % COLS;
    const crit = getCriticalMass(rr, cc);
    if (board.counts[idx] >= crit) {
      board.counts[idx] -= crit;
      if (board.counts[idx] === 0) board.colors[idx] = 0;

      [ [1,0],[-1,0],[0,1],[0,-1] ].forEach(([dr, dc]) => {
        const nr = rr + dr, nc = cc + dc;
        if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS) return;
        const ni = index(nr, nc);
        board.counts[ni] += 1;
        board.colors[ni]  = playerColor;
        if (board.counts[ni] >= getCriticalMass(nr, nc)) {
          queue.push(ni);
        }
      });
    }
  }
}

function isGameOver(board) {
  let seenR = false, seenB = false;
  for (let i = 0; i < CELLS; i++) {
    if (board.colors[i] === 1) seenR = true;
    if (board.colors[i] === 2) seenB = true;
    if (seenR && seenB) return null;
  }
  if (seenR) return 'R';
  if (seenB) return 'B';
  return null;
}

function getLegalMoves(board, playerColor) {
  const moves = [];
  for (let r = 0; r < ROWS; r++) {
    for (let c = 0; c < COLS; c++) {
      const i = index(r, c);
      if (board.colors[i] === 0 || board.colors[i] === playerColor) {
        moves.push({ r, c });
      }
    }
  }
  return moves;
}


function smartHeuristic(board) {
  let score = 0;
  for (let r = 0; r < ROWS; r++) {
    for (let c = 0; c < COLS; c++) {
      const i   = index(r, c);
      const cnt = board.counts[i];
      const col = board.colors[i];
      if (col === 0) continue;

      const crit = getCriticalMass(r, c);
      let weight = cnt * 10;

      const isCorner = ((r === 0 || r === ROWS-1) && (c === 0 || c === COLS-1));
      const isEdge   = ((r === 0 || r === ROWS-1) || (c === 0 || c === COLS-1));
      if (isCorner)     weight += 5;
      else if (isEdge)  weight += 3;

      if (cnt === crit - 1)      weight += 6;
      else if (cnt === crit - 2) weight += 3;


      [ [-1,0],[1,0],[0,-1],[0,1] ].forEach(([dr,dc]) => {
        const nr = r + dr, nc = c + dc;
        if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS) return;
        const ni   = index(nr, nc);
        const ncol = board.colors[ni], ncnt = board.counts[ni];
        if (col === 2 && ncol === 1 && ncnt >= getCriticalMass(nr,nc)-1) weight -= 8;
        if (col === 1 && ncol === 2 && ncnt >= crit - 1)                 weight += 8;
      });

      score += (col === 2 ? weight : -weight);
    }
  }
  return score;
}

function minimax(board, depth, alpha, beta, maximizing) {
  const win = isGameOver(board);
  if (win || depth === 0) {
    if (win === 'B') return  1e6;
    if (win === 'R') return -1e6;
    return smartHeuristic(board);
  }

  const playerColor = maximizing ? 2 : 1;
  let best = maximizing ? -Infinity : Infinity;
  const moves = getLegalMoves(board, playerColor);

  for (const { r, c } of moves) {
    // fast clone via slice()
    const b2 = {
      counts: board.counts.slice(),
      colors: board.colors.slice()
    };
    applyMove(b2, r, c, playerColor);

    const val = minimax(b2, depth - 1, alpha, beta, !maximizing);
    if (maximizing) {
      best  = Math.max(best, val);
      alpha = Math.max(alpha, val);
    } else {
      best  = Math.min(best, val);
      beta  = Math.min(beta, val);
    }
    if (beta <= alpha) break;
  }

  return best;
}

function minimaxSearch(board) {
  let bestMove = { r: -1, c: -1 };
  let bestVal  = -Infinity;

  for (const { r, c } of getLegalMoves(board, 2)) {
    const b2 = {
      counts: board.counts.slice(),
      colors: board.colors.slice()
    };
    applyMove(b2, r, c, 2);
    const val = minimax(b2, MAX_DEPTH - 1, -Infinity, Infinity, false);
    if (val > bestVal) {
      bestVal  = val;
      bestMove = { r, c };
    }
  }

  return bestMove;
}

module.exports = {
  createBoard,
  applyMove,
  isGameOver,
  minimaxSearch
};
