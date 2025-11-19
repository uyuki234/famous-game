// reversi.cpp - コンソール版オセロ（合法手・反転・簡易AI付き）
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
using namespace std;

static const int N = 8;
static const char EMPTY = '.';
static const char BLACK = 'X'; // 先手
static const char WHITE = 'O'; // 後手

struct Move { int r, c; }; // r: 0-7 (行:1-8), c: 0-7 (列:a-h)

bool inBounds(int r, int c) { return r >= 0 && r < N && c >= 0 && c < N; }
char opponent(char p) { return p == BLACK ? WHITE : BLACK; }

void printBoard(const vector<vector<char>>& b) {
    cout << "\n   a b c d e f g h\n";
    for (int r = 0; r < N; ++r) {
        cout << (r+1) << "  ";
        for (int c = 0; c < N; ++c) {
            cout << b[r][c];
            if (c < N-1) cout << ' ';
        }
        cout << "\n";
    }
    cout << "\n(X=黒, O=白)\n";
}

// 置いたときに反転される座標の一覧を返す（反転可能なら1つ以上返る）
vector<Move> findFlips(const vector<vector<char>>& b, char p, int r, int c) {
    vector<Move> flips;
    if (!inBounds(r,c) || b[r][c] != EMPTY) return flips;

    static const int dr[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    static const int dc[8] = {-1, 0, 1,-1, 1,-1, 0, 1};
    char opp = opponent(p);

    for (int d = 0; d < 8; ++d) {
        int nr = r + dr[d], nc = c + dc[d];
        vector<Move> line;
        // まず隣が相手である必要
        if (!inBounds(nr,nc) || b[nr][nc] != opp) continue;
        // 相手石が連続する限り進み、最後が自分の石で止まればその間が反転対象
        while (inBounds(nr,nc) && b[nr][nc] == opp) {
            line.push_back({nr,nc});
            nr += dr[d]; nc += dc[d];
        }
        if (inBounds(nr,nc) && b[nr][nc] == p && !line.empty()) {
            // 反転確定
            flips.insert(flips.end(), line.begin(), line.end());
        }
    }
    return flips; // 空なら反転なし＝非合法
}

bool isLegal(const vector<vector<char>>& b, char p, int r, int c) {
    return !findFlips(b, p, r, c).empty();
}

vector<Move> legalMoves(const vector<vector<char>>& b, char p) {
    vector<Move> moves;
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            if (isLegal(b, p, r, c)) moves.push_back({r,c});
    return moves;
}

void applyMove(vector<vector<char>>& b, char p, int r, int c) {
    auto flips = findFlips(b, p, r, c);
    b[r][c] = p;
    for (auto &m : flips) b[m.r][m.c] = p;
}

pair<int,int> countDiscs(const vector<vector<char>>& b) {
    int x=0, o=0;
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            if (b[r][c] == BLACK) ++x;
            else if (b[r][c] == WHITE) ++o;
    return {x,o};
}

// 盤面の評価用重み（シンプル版：角に高評価, 隅や危険マスは低評価）
static const int W[N][N] = {
    { 120,-20, 20,  5,  5, 20,-20,120},
    { -20,-40, -5, -5, -5, -5,-40,-20},
    {  20,  -5, 15,  3,  3, 15,  -5, 20},
    {   5,  -5,  3,  3,  3,  3,  -5,  5},
    {   5,  -5,  3,  3,  3,  3,  -5,  5},
    {  20,  -5, 15,  3,  3, 15,  -5, 20},
    { -20,-40, -5, -5, -5, -5,-40,-20},
    { 120,-20, 20,  5,  5, 20,-20,120}
};

// 簡易AI：角を最優先 → 反転枚数の多い手 → 位置重みの高い手
Move chooseMoveAI(const vector<vector<char>>& b, char ai) {
    auto moves = legalMoves(b, ai);
    if (moves.empty()) return {-1,-1};
    char opp = opponent(ai);

    // 角があれば即選択
    for (auto m : moves) {
        if ((m.r == 0 && m.c == 0) || (m.r == 0 && m.c == 7) ||
            (m.r == 7 && m.c == 0) || (m.r == 7 && m.c == 7)) {
            return m;
        }
    }

    // 反転枚数 + 位置評価でスコア化
    int bestScore = -1e9;
    Move best = moves.front();
    for (auto m : moves) {
        auto flips = findFlips(b, ai, m.r, m.c);
        int flipCount = (int)flips.size();
        int posScore  = W[m.r][m.c];

        // 1手先の相手合法手の数（与える手数を少なく）も少し考慮
        vector<vector<char>> tmp = b;
        applyMove(tmp, ai, m.r, m.c);
        int oppMobility = (int)legalMoves(tmp, opp).size();

        int score = flipCount * 10 + posScore - oppMobility * 2;
        if (score > bestScore) { bestScore = score; best = m; }
    }
    return best;
}

// 入力 "d3" / "D3" / "3d" を受け付ける（列[a-h], 行[1-8]）。"pass" / "q"もOK。
bool parseMove(const string& s, Move& out) {
    if (s == "pass" || s == "PASS") { out = {-1,-1}; return true; }
    if (s == "q" || s == "Q")       { out = {-2,-2}; return true; }

    int row = -1, col = -1;
    for (char ch : s) {
        if (isalpha((unsigned char)ch)) {
            ch = tolower(ch);
            if (ch >= 'a' && ch <= 'h') col = ch - 'a';
        } else if (isdigit((unsigned char)ch)) {
            int v = ch - '0';
            if (v >= 1 && v <= 8) row = v - 1;
        }
    }
    if (row >= 0 && col >= 0) { out = {row, col}; return true; }
    return false;
}

int main() {
    // 初期配置
    vector<vector<char>> b(N, vector<char>(N, EMPTY));
    b[3][3] = WHITE; b[4][4] = WHITE;
    b[3][4] = BLACK; b[4][3] = BLACK;
    char turn = BLACK; // 先手は黒

    cout << "Othello / Reversi (Console)\n";
    cout << "入力例: d3（列a-h, 行1-8）。パス: pass, 終了: q\n";
    cout << "あなた=黒(X), AI=白(O)\n";

    bool lastPass = false; // 直前にパスしたか
    while (true) {
        printBoard(b);
        auto [bx, bo] = countDiscs(b);
        cout << "石数: 黒(X)=" << bx << " 白(O)=" << bo << "\n";

        auto moves = legalMoves(b, turn);
        if (moves.empty()) {
            cout << (turn==BLACK ? "黒" : "白") << " に合法手がありません → パス\n";
            if (lastPass) {
                cout << "両者とも打てないため、ゲーム終了。\n";
                break;
            }
            lastPass = true;
            turn = opponent(turn);
            continue;
        }
        lastPass = false;

        if (turn == BLACK) {
            // 人間の手番
            cout << "あなた(黒)の手番です。入力してください (例: d3 / pass / q): ";
            string s;
            if (!getline(cin, s)) { cout << "入力を終了します。\n"; break; }
            if (s.empty()) continue;

            Move m;
            if (!parseMove(s, m)) {
                cout << "形式が不正です。列[a-h]と行[1-8]の組み合わせで入力してください。\n";
                continue;
            }
            if (m.r == -2 && m.c == -2) { // q
                cout << "終了します。\n"; break;
            }
            if (m.r == -1 && m.c == -1) { // pass
                // 合法手があるのにパスは不可（オセロのルール）
                cout << "合法手があります。パスはできません。\n";
                continue;
            }
            if (!isLegal(b, turn, m.r, m.c)) {
                cout << "その手は合法ではありません。\n";
                continue;
            }
            applyMove(b, turn, m.r, m.c);
            turn = opponent(turn);
        } else {
            // AIの手番（白）
            Move aiMove = chooseMoveAI(b, WHITE);
            if (aiMove.r == -1 && aiMove.c == -1) {
                cout << "AI(白)はパスします。\n";
                lastPass = (turn == WHITE && moves.empty());
                turn = opponent(turn);
            } else {
                cout << "AI(白)の手: " << char('a' + aiMove.c) << (aiMove.r + 1) << "\n";
                applyMove(b, WHITE, aiMove.r, aiMove.c);
                turn = opponent(turn);
            }
        }

        // 全面が埋まったら終了
        auto [x,o] = countDiscs(b);
        if (x + o == N*N) {
            cout << "盤が埋まりました。ゲーム終了。\n";
            break;
        }
    }

    printBoard(b);
    auto [x,o] = countDiscs(b);
    cout << "最終結果: 黒(X)=" << x << " 白(O)=" << o << "\n";
    if (x > o) cout << "あなた(黒)の勝ち！\n";
    else if (o > x) cout << "AI(白)の勝ち！\n";
    else cout << "引き分け！\n";

    return 0;
    }

