#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
三目並べ (Tic-Tac-Toe) - Python実装
シンプルなコマンドライン版の三目並べゲーム
"""


class TicTacToe:
    def __init__(self):
        """ゲームの初期化"""
        self.board = [' ' for _ in range(9)]
        self.current_player = 'X'
        
    def display_board(self):
        """ボードを表示"""
        print("\n")
        print(f" {self.board[0]} | {self.board[1]} | {self.board[2]} ")
        print("---|---|---")
        print(f" {self.board[3]} | {self.board[4]} | {self.board[5]} ")
        print("---|---|---")
        print(f" {self.board[6]} | {self.board[7]} | {self.board[8]} ")
        print("\n")
        
    def display_positions(self):
        """位置番号を表示"""
        print("\n位置番号:")
        print(" 1 | 2 | 3 ")
        print("---|---|---")
        print(" 4 | 5 | 6 ")
        print("---|---|---")
        print(" 7 | 8 | 9 ")
        print()
        
    def is_valid_move(self, position):
        """移動が有効かチェック"""
        return 0 <= position < 9 and self.board[position] == ' '
    
    def make_move(self, position):
        """移動を実行"""
        if self.is_valid_move(position):
            self.board[position] = self.current_player
            return True
        return False
    
    def check_winner(self):
        """勝者をチェック"""
        # 勝利パターン
        winning_combinations = [
            [0, 1, 2], [3, 4, 5], [6, 7, 8],  # 横
            [0, 3, 6], [1, 4, 7], [2, 5, 8],  # 縦
            [0, 4, 8], [2, 4, 6]               # 斜め
        ]
        
        for combo in winning_combinations:
            if (self.board[combo[0]] == self.board[combo[1]] == 
                self.board[combo[2]] != ' '):
                return self.board[combo[0]]
        return None
    
    def is_board_full(self):
        """ボードが埋まっているかチェック"""
        return ' ' not in self.board
    
    def switch_player(self):
        """プレイヤーを切り替え"""
        self.current_player = 'O' if self.current_player == 'X' else 'X'
    
    def play(self):
        """ゲームを実行"""
        print("=" * 40)
        print("三目並べ (Tic-Tac-Toe) へようこそ!")
        print("=" * 40)
        self.display_positions()
        
        while True:
            self.display_board()
            print(f"プレイヤー {self.current_player} の番です")
            
            try:
                position = int(input("位置を選択 (1-9): ")) - 1
                
                if not self.is_valid_move(position):
                    print("無効な移動です。もう一度試してください。")
                    continue
                
                self.make_move(position)
                
                # 勝者チェック
                winner = self.check_winner()
                if winner:
                    self.display_board()
                    print(f"🎉 プレイヤー {winner} の勝ちです!")
                    break
                
                # 引き分けチェック
                if self.is_board_full():
                    self.display_board()
                    print("引き分けです!")
                    break
                
                # プレイヤー切り替え
                self.switch_player()
                
            except (ValueError, IndexError):
                print("1から9の数字を入力してください。")
            except KeyboardInterrupt:
                print("\n\nゲームを終了します。")
                break


def main():
    """メイン関数"""
    while True:
        game = TicTacToe()
        game.play()
        
        play_again = input("\nもう一度プレイしますか? (y/n): ").lower()
        if play_again != 'y':
            print("ありがとうございました!")
            break


if __name__ == "__main__":
    main()
