#include <iostream>
#include <vector>
#include <cstring>
#include <limits>
using namespace std;

char board[9];

void printBoard() {
    cout << "\n";
    for (int i = 0; i < 9; i++) {
        cout << (board[i] ? board[i] : char('1' + i));
        if ((i + 1) % 3 == 0) cout << "\n";
        else cout << " | ";
    }
    cout << "\n";
}

bool checkWin(char p) {
    int w[8][3] = {
        {0,1,2},{3,4,5},{6,7,8}, // rows
        {0,3,6},{1,4,7},{2,5,8}, // cols
        {0,4,8},{2,4,6}          // diags
    };
    for (auto &line : w) {
        if (board[line[0]] == p && board[line[1]] == p && board[line[2]] == p) return true;
    }
    return false;
}

bool isFull() {
    for (char c : board) if (c == 0) return false;
    return true;
}

// すごく簡単なAI：勝てる手→ブロック→中央→隅→適当
int chooseMoveAI(char ai='O', char human='X') {
    auto canWin = [](char player) -> int {
        for (int i = 0; i < 9; i++) if (board[i]==0){
            board[i] = player;
            bool win = checkWin(player);
            board[i] = 0;
            if (win) return i;
        }
        return -1;
    };
    int move;
    if ((move = canWin(ai)) != -1) return move;            // 勝ち
    if ((move = canWin(human)) != -1) return move;         // ブロック
    if (board[4]==0) return 4;                             // 中央
    vector<int> corners = {0,2,6,8};
    for (int c : corners) if (board[c]==0) return c;       // 隅
    for (int i = 0; i < 9; i++) if (board[i]==0) return i; // その他
    return -1;
}

int main(){
    memset(board, 0, sizeof(board));
    cout << "Tic-Tac-Toe (あなた: X / AI: O)\n";
    cout << "番号(1-9)でマスを選んでください。左上=1, 右下=9\n";

    char turn = 'X';
    while (true) {
        printBoard();
        if (turn == 'X') {
            int pos;
            cout << "あなたの手番です。入力(1-9): ";
            if (!(cin >> pos) || pos < 1 || pos > 9 || board[pos-1] != 0) {
                cout << "無効な入力です。\n";
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            board[pos-1] = 'X';
            if (checkWin('X')) { printBoard(); cout << "あなたの勝ち！\n"; break; }
        } else {
            int aiMove = chooseMoveAI();
            board[aiMove] = 'O';
            cout << "AIの手: " << (aiMove+1) << "\n";
            if (checkWin('O')) { printBoard(); cout << "AIの勝ち...\n"; break; }
        }
        if (isFull()) { printBoard(); cout << "引き分け。\n"; break; }
        turn = (turn=='X') ? 'O' : 'X';
    }
    return 0;
}
