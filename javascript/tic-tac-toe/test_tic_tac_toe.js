#!/usr/bin/env node
/**
 * 三目並べ (Tic-Tac-Toe) のテスト
 * 基本的なゲームロジックをテスト
 */

const TicTacToe = require('./tic_tac_toe');

/**
 * アサーション関数
 */
function assert(condition, message) {
  if (!condition) {
    throw new Error(message);
  }
}

/**
 * 配列が等しいかチェック
 */
function arrayEquals(a, b) {
  return a.length === b.length && a.every((val, index) => val === b[index]);
}

/**
 * 初期化テスト
 */
function testInitialization() {
  const game = new TicTacToe();
  assert(arrayEquals(game.board, Array(9).fill(' ')), 'ボードは空であるべき');
  assert(game.currentPlayer === 'X', '最初のプレイヤーはXであるべき');
  game.close();
  console.log('✓ 初期化テスト: 成功');
}

/**
 * 有効な移動のテスト
 */
function testValidMove() {
  const game = new TicTacToe();
  assert(game.makeMove(0) === true, '空のマスへの移動は成功すべき');
  assert(game.board[0] === 'X', 'マスにXが置かれるべき');
  game.close();
  console.log('✓ 有効な移動テスト: 成功');
}

/**
 * 無効な移動のテスト
 */
function testInvalidMove() {
  const game = new TicTacToe();
  game.makeMove(0);
  assert(game.makeMove(0) === false, '既に置かれたマスへの移動は失敗すべき');
  game.close();
  console.log('✓ 無効な移動テスト: 成功');
}

/**
 * 横一列の勝利テスト
 */
function testHorizontalWin() {
  const game = new TicTacToe();
  // X が横一列を揃える
  game.makeMove(0);  // X
  game.switchPlayer();
  game.makeMove(3);  // O
  game.switchPlayer();
  game.makeMove(1);  // X
  game.switchPlayer();
  game.makeMove(4);  // O
  game.switchPlayer();
  game.makeMove(2);  // X (勝利)
  
  const winner = game.checkWinner();
  assert(winner === 'X', 'Xが勝者であるべき');
  game.close();
  console.log('✓ 横一列の勝利テスト: 成功');
}

/**
 * 縦一列の勝利テスト
 */
function testVerticalWin() {
  const game = new TicTacToe();
  // X が縦一列を揃える
  game.makeMove(0);  // X
  game.switchPlayer();
  game.makeMove(1);  // O
  game.switchPlayer();
  game.makeMove(3);  // X
  game.switchPlayer();
  game.makeMove(2);  // O
  game.switchPlayer();
  game.makeMove(6);  // X (勝利)
  
  const winner = game.checkWinner();
  assert(winner === 'X', 'Xが勝者であるべき');
  game.close();
  console.log('✓ 縦一列の勝利テスト: 成功');
}

/**
 * 斜めの勝利テスト
 */
function testDiagonalWin() {
  const game = new TicTacToe();
  // X が斜めに揃える
  game.makeMove(0);  // X
  game.switchPlayer();
  game.makeMove(1);  // O
  game.switchPlayer();
  game.makeMove(4);  // X
  game.switchPlayer();
  game.makeMove(2);  // O
  game.switchPlayer();
  game.makeMove(8);  // X (勝利)
  
  const winner = game.checkWinner();
  assert(winner === 'X', 'Xが勝者であるべき');
  game.close();
  console.log('✓ 斜めの勝利テスト: 成功');
}

/**
 * 引き分けのテスト
 */
function testDraw() {
  const game = new TicTacToe();
  // 引き分けになるパターン
  const moves = [0, 1, 2, 4, 3, 5, 7, 6, 8];
  for (let i = 0; i < moves.length; i++) {
    game.makeMove(moves[i]);
    if (i < moves.length - 1) {
      game.switchPlayer();
    }
  }
  
  assert(game.isBoardFull() === true, 'ボードは満杯であるべき');
  assert(game.checkWinner() === null, '勝者はいないべき');
  game.close();
  console.log('✓ 引き分けテスト: 成功');
}

/**
 * プレイヤー切り替えテスト
 */
function testPlayerSwitching() {
  const game = new TicTacToe();
  assert(game.currentPlayer === 'X', '最初のプレイヤーはX');
  game.switchPlayer();
  assert(game.currentPlayer === 'O', '切り替え後はO');
  game.switchPlayer();
  assert(game.currentPlayer === 'X', '再切り替え後はX');
  game.close();
  console.log('✓ プレイヤー切り替えテスト: 成功');
}

/**
 * すべてのテストを実行
 */
function runAllTests() {
  console.log('==================================================');
  console.log('三目並べ テスト実行中...');
  console.log('==================================================');
  
  try {
    testInitialization();
    testValidMove();
    testInvalidMove();
    testHorizontalWin();
    testVerticalWin();
    testDiagonalWin();
    testDraw();
    testPlayerSwitching();
    
    console.log('\n==================================================');
    console.log('✓ すべてのテストが成功しました!');
    console.log('==================================================');
    return true;
    
  } catch (error) {
    console.log(`\n✗ テストが失敗しました: ${error.message}`);
    console.error(error);
    return false;
  }
}

// テストを実行
if (require.main === module) {
  const success = runAllTests();
  process.exit(success ? 0 : 1);
}

module.exports = { runAllTests };
