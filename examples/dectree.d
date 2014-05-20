[e] = entropy(board, a, b){
  entropy = 0;
  board<i><j>{
    entropy = entropy + a * (board[i][j] != 0 ?
      (board[i+1][j] != 0 ? (board[i][j] - board[i+1][j])^2 : 0) +
      (board[i-1][j] != 0 ? (board[i][j] - board[i-1][j])^2 : 0) +
      (board[i][j+1] != 0 ? (board[i][j] - board[i][j+1])^2 : 0) +
      (board[i][j-1] != 0 ? (board[i][j] - board[i][j-1])^2 : 0)
      : 0);
    entropy = entropy + b * max(board);
  }
}
