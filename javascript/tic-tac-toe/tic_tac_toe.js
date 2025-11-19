#!/usr/bin/env node
/**
 * 三目並べ (Tic-Tac-Toe) - JavaScript実装
 * Node.jsで動作するコマンドライン版の三目並べゲーム
 */

const readline = require('readline');

class TicTacToe {
  constructor() {
    this.board = Array(9).fill(' ');
    this.currentPlayer = 'X';
    this.rl = readline.createInterface({
      input: process.stdin,
      output: process.stdout
    });
  }

  /**
   * ボードを表示
   */
  displayBoard() {
    console.log('\n');
    console.log(` ${this.board[0]} | ${this.board[1]} | ${this.board[2]} `);
    console.log('---|---|---');
    console.log(` ${this.board[3]} | ${this.board[4]} | ${this.board[5]} `);
    console.log('---|---|---');
    console.log(` ${this.board[6]} | ${this.board[7]} | ${this.board[8]} `);
    console.log('\n');
  }

  /**
   * 位置番号を表示
   */
  displayPositions() {
    console.log('\n位置番号:');
    console.log(' 1 | 2 | 3 ');
    console.log('---|---|---');
    console.log(' 4 | 5 | 6 ');
    console.log('---|---|---');
    console.log(' 7 | 8 | 9 ');
    console.log();
  }

  /**
   * 移動が有効かチェック
   */
  isValidMove(position) {
    return position >= 0 && position < 9 && this.board[position] === ' ';
  }

  /**
   * 移動を実行
   */
  makeMove(position) {
    if (this.isValidMove(position)) {
      this.board[position] = this.currentPlayer;
      return true;
    }
    return false;
  }

  /**
   * 勝者をチェック
   */
  checkWinner() {
    const winningCombinations = [
      [0, 1, 2], [3, 4, 5], [6, 7, 8], // 横
      [0, 3, 6], [1, 4, 7], [2, 5, 8], // 縦
      [0, 4, 8], [2, 4, 6]              // 斜め
    ];

    for (const combo of winningCombinations) {
      if (this.board[combo[0]] === this.board[combo[1]] &&
          this.board[combo[1]] === this.board[combo[2]] &&
          this.board[combo[0]] !== ' ') {
        return this.board[combo[0]];
      }
    }
    return null;
  }

  /**
   * ボードが埋まっているかチェック
   */
  isBoardFull() {
    return !this.board.includes(' ');
  }

  /**
   * プレイヤーを切り替え
   */
  switchPlayer() {
    this.currentPlayer = this.currentPlayer === 'X' ? 'O' : 'X';
  }

  /**
   * ユーザー入力を取得
   */
  async getInput(question) {
    return new Promise((resolve) => {
      this.rl.question(question, (answer) => {
        resolve(answer);
      });
    });
  }

  /**
   * ゲームメインループ
   */
  async play() {
    console.log('========================================');
    console.log('三目並べ (Tic-Tac-Toe) へようこそ!');
    console.log('========================================');
    this.displayPositions();

    while (true) {
      this.displayBoard();
      console.log(`プレイヤー ${this.currentPlayer} の番です`);

      const input = await this.getInput('位置を選択 (1-9): ');
      const position = parseInt(input) - 1;

      if (isNaN(position) || !this.isValidMove(position)) {
        console.log('無効な移動です。もう一度試してください。');
        continue;
      }

      this.makeMove(position);

      // 勝者チェック
      const winner = this.checkWinner();
      if (winner) {
        this.displayBoard();
        console.log(`🎉 プレイヤー ${winner} の勝ちです!`);
        break;
      }

      // 引き分けチェック
      if (this.isBoardFull()) {
        this.displayBoard();
        console.log('引き分けです!');
        break;
      }

      // プレイヤー切り替え
      this.switchPlayer();
    }
  }

  /**
   * リソースをクリーンアップ
   */
  close() {
    this.rl.close();
  }
}

/**
 * メイン関数
 */
async function main() {
  while (true) {
    const game = new TicTacToe();
    
    try {
      await game.play();
      
      const playAgain = await game.getInput('\nもう一度プレイしますか? (y/n): ');
      game.close();
      
      if (playAgain.toLowerCase() !== 'y') {
        console.log('ありがとうございました!');
        break;
      }
    } catch (error) {
      game.close();
      console.log('\n\nゲームを終了します。');
      break;
    }
  }
  
  process.exit(0);
}

// プログラムを実行
if (require.main === module) {
  main().catch((error) => {
    console.error('エラーが発生しました:', error);
    process.exit(1);
  });
}

module.exports = TicTacToe;
