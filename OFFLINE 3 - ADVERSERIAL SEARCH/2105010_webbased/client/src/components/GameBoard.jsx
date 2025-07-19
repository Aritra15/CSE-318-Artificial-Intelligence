import React from 'react';

export default function GameBoard({ board, onCellClick }) {
  return (
    <div className="board">
      {board.map((row, r) => (
        <div className="row" key={r}>
          {row.map((cell, c) => (
            <div
              key={c}
              className={`cell ${cell.color === 'R' ? 'red' : cell.color === 'B' ? 'blue' : ''}`}
              onClick={() => onCellClick(r, c)}
            >
              {cell.count > 0 ? `${cell.count}` : ''}
            </div>
          ))}
        </div>
      ))}
    </div>
  );
}