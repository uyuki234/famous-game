#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
三目並べ (Tic-Tac-Toe) のテスト
基本的なゲームロジックをテスト
"""

import sys
import os

# 親ディレクトリからインポートできるようにパスを追加
sys.path.insert(0, os.path.dirname(__file__))

from tic_tac_toe import TicTacToe


def test_initialization():
    """初期化テスト"""
    game = TicTacToe()
    assert game.board == [' '] * 9, "ボードは空であるべき"
    assert game.current_player == 'X', "最初のプレイヤーはXであるべき"
    print("✓ 初期化テスト: 成功")


def test_valid_move():
    """有効な移動のテスト"""
    game = TicTacToe()
    assert game.make_move(0) == True, "空のマスへの移動は成功すべき"
    assert game.board[0] == 'X', "マスにXが置かれるべき"
    print("✓ 有効な移動テスト: 成功")


def test_invalid_move():
    """無効な移動のテスト"""
    game = TicTacToe()
    game.make_move(0)
    assert game.make_move(0) == False, "既に置かれたマスへの移動は失敗すべき"
    print("✓ 無効な移動テスト: 成功")


def test_horizontal_win():
    """横一列の勝利テスト"""
    game = TicTacToe()
    # X が横一列を揃える
    game.make_move(0)  # X
    game.switch_player()
    game.make_move(3)  # O
    game.switch_player()
    game.make_move(1)  # X
    game.switch_player()
    game.make_move(4)  # O
    game.switch_player()
    game.make_move(2)  # X (勝利)
    
    winner = game.check_winner()
    assert winner == 'X', "Xが勝者であるべき"
    print("✓ 横一列の勝利テスト: 成功")


def test_vertical_win():
    """縦一列の勝利テスト"""
    game = TicTacToe()
    # X が縦一列を揃える
    game.make_move(0)  # X
    game.switch_player()
    game.make_move(1)  # O
    game.switch_player()
    game.make_move(3)  # X
    game.switch_player()
    game.make_move(2)  # O
    game.switch_player()
    game.make_move(6)  # X (勝利)
    
    winner = game.check_winner()
    assert winner == 'X', "Xが勝者であるべき"
    print("✓ 縦一列の勝利テスト: 成功")


def test_diagonal_win():
    """斜めの勝利テスト"""
    game = TicTacToe()
    # X が斜めに揃える
    game.make_move(0)  # X
    game.switch_player()
    game.make_move(1)  # O
    game.switch_player()
    game.make_move(4)  # X
    game.switch_player()
    game.make_move(2)  # O
    game.switch_player()
    game.make_move(8)  # X (勝利)
    
    winner = game.check_winner()
    assert winner == 'X', "Xが勝者であるべき"
    print("✓ 斜めの勝利テスト: 成功")


def test_draw():
    """引き分けのテスト"""
    game = TicTacToe()
    # 引き分けになるパターン
    moves = [0, 1, 2, 4, 3, 5, 7, 6, 8]
    for i, move in enumerate(moves):
        game.make_move(move)
        if i < len(moves) - 1:
            game.switch_player()
    
    assert game.is_board_full() == True, "ボードは満杯であるべき"
    assert game.check_winner() is None, "勝者はいないべき"
    print("✓ 引き分けテスト: 成功")


def test_player_switching():
    """プレイヤー切り替えテスト"""
    game = TicTacToe()
    assert game.current_player == 'X', "最初のプレイヤーはX"
    game.switch_player()
    assert game.current_player == 'O', "切り替え後はO"
    game.switch_player()
    assert game.current_player == 'X', "再切り替え後はX"
    print("✓ プレイヤー切り替えテスト: 成功")


def run_all_tests():
    """すべてのテストを実行"""
    print("=" * 50)
    print("三目並べ テスト実行中...")
    print("=" * 50)
    
    try:
        test_initialization()
        test_valid_move()
        test_invalid_move()
        test_horizontal_win()
        test_vertical_win()
        test_diagonal_win()
        test_draw()
        test_player_switching()
        
        print("\n" + "=" * 50)
        print("✓ すべてのテストが成功しました!")
        print("=" * 50)
        return True
        
    except AssertionError as e:
        print(f"\n✗ テストが失敗しました: {e}")
        return False


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
