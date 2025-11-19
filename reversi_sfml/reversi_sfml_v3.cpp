// reversi_sfml_v3.cpp - SFML v3 対応版のオセロ雛形
// macOS: brew install sfml
// ビルド: clang++ -std=c++17 -O2 reversi_sfml_v3.cpp -o reversi_sfml \
//   $(pkg-config --cflags --libs sfml-all)

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

static const int N = 8;
static const char EMPTY = '.';
static const char BLACK = 'X'; // 先手（あなた）
static const char WHITE = 'O'; // 後手（AIにしたければ後述のフラグを有効化）

struct Move { int r, c; };

bool inBounds(int r, int c) { return r >= 0 && r < N && c >= 0 && c < N; }
char opponent(char p) { return p == BLACK ? WHITE : BLACK; }

// 反転対象の一覧を返す
std::vector<Move> findFlips(const std::vector<std::vector<char>>& b, char p, int r, int c) {
    std::vector<Move> flips;
    if (!inBounds(r,c) || b[r][c] != EMPTY) return flips;

    static const int dr[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    static const int dc[8] = {-1, 0, 1,-1, 1,-1, 0, 1};
    char opp = opponent(p);

    for (int d = 0; d < 8; ++d) {
        int nr = r + dr[d], nc = c + dc[d];
        std::vector<Move> line;
        if (!inBounds(nr,nc) || b[nr][nc] != opp) continue;
        while (inBounds(nr,nc) && b[nr][nc] == opp) {
            line.push_back({nr,nc});
            nr += dr[d]; nc += dc[d];
        }
        if (inBounds(nr,nc) && b[nr][nc] == p && !line.empty()) {
            flips.insert(flips.end(), line.begin(), line.end());
        }
    }
    return flips;
}

bool isLegal(const std::vector<std::vector<char>>& b, char p, int r, int c) {
    return !findFlips(b, p, r, c).empty();
}

std::vector<Move> legalMoves(const std::vector<std::vector<char>>& b, char p) {
    std::vector<Move> moves;
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            if (isLegal(b, p, r, c)) moves.push_back({r,c});
    return moves;
}

void applyMove(std::vector<std::vector<char>>& b, char p, int r, int c) {
    auto flips = findFlips(b, p, r, c);
    b[r][c] = p;
    for (auto &m : flips) b[m.r][m.c] = p;
}

std::pair<int,int> countDiscs(const std::vector<std::vector<char>>& b) {
    int x=0, o=0;
    for (int r=0; r<N; ++r) for (int c=0; c<N; ++c) {
        x += (b[r][c] == BLACK);
        o += (b[r][c] == WHITE);
    }
    return {x,o};
}

// --- 簡易AI（角優先＋反転枚数＋位置重み）--- 必要ならONにして使えます
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

Move chooseMoveAI(const std::vector<std::vector<char>>& b, char ai) {
    auto moves = legalMoves(b, ai);
    if (moves.empty()) return {-1,-1};
    for (auto m : moves) {
        if ((m.r==0 && m.c==0) || (m.r==0 && m.c==7) || (m.r==7 && m.c==0) || (m.r==7 && m.c==7))
            return m;
    }
    int bestScore = -1000000;
    Move best = moves.front();
    char opp = opponent(ai);
    for (auto m : moves) {
        auto flips = findFlips(b, ai, m.r, m.c);
        int flipCount = (int)flips.size();
        int posScore  = W[m.r][m.c];
        std::vector<std::vector<char>> tmp = b;
        applyMove(tmp, ai, m.r, m.c);
        int oppMobility = (int)legalMoves(tmp, opp).size();
        int score = flipCount*10 + posScore - oppMobility*2;
        if (score > bestScore) { bestScore = score; best = m; }
    }
    return best;
}

// --- 描画関連 ---
struct UI {
    int CELL = 80;         // 1マスのピクセル
    int MARGIN = 20;       // 盤の外余白
    int W, H;

    UI() { W = MARGIN*2 + CELL*N; H = MARGIN*2 + CELL*N; }

    sf::Vector2f cellTopLeft(int r, int c) const {
        return {(float)(MARGIN + c*CELL), (float)(MARGIN + r*CELL)};
    }
    bool posToRC(sf::Vector2i pos, int &r, int &c) const {
        int x = pos.x - MARGIN, y = pos.y - MARGIN;
        if (x < 0 || y < 0) return false;
        c = x / CELL; r = y / CELL;
        return inBounds(r,c);
    }
};

int main() {
    // 盤の初期化
    std::vector<std::vector<char>> b(N, std::vector<char>(N, EMPTY));
    b[3][3] = WHITE; b[4][4] = WHITE;
    b[3][4] = BLACK; b[4][3] = BLACK;

    char turn = BLACK;
    bool lastPass = false;
    bool gameOver = false;

    // 雛形では人対人（両者クリック）で動きます。
    // 白を簡易AIにしたい場合は次のフラグを true にしてください。
    bool aiWhite = false;

    UI ui;
    sf::RenderWindow win(sf::VideoMode(sf::Vector2u((unsigned)ui.W, (unsigned)ui.H)), "Reversi (SFML) - Turn: Black");
    win.setFramerateLimit(60);

    // 緑の盤背景
    sf::RectangleShape boardBg(sf::Vector2f((float)(ui.CELL*N), (float)(ui.CELL*N)));
    boardBg.setPosition(sf::Vector2f((float)ui.MARGIN, (float)ui.MARGIN));
    boardBg.setFillColor(sf::Color(0, 120, 0));

    // グリッド線
    std::vector<sf::RectangleShape> gridLines;
    for (int i = 0; i <= N; ++i) {
        sf::RectangleShape h(sf::Vector2f((float)(ui.CELL*N), 2.f));
        h.setPosition(sf::Vector2f((float)ui.MARGIN, (float)(ui.MARGIN + i*ui.CELL)));
        h.setFillColor(sf::Color(0, 80, 0));
        gridLines.push_back(h);

        sf::RectangleShape v(sf::Vector2f(2.f, (float)(ui.CELL*N)));
        v.setPosition(sf::Vector2f((float)(ui.MARGIN + i*ui.CELL), (float)ui.MARGIN));
        v.setFillColor(sf::Color(0, 80, 0));
        gridLines.push_back(v);
    }

    // 石（円）
    sf::CircleShape disc((float)(ui.CELL*0.45));
    disc.setOrigin(sf::Vector2f(disc.getRadius(), disc.getRadius()));

    // 合法手ハイライト（半透明の小さな点）
    sf::CircleShape hint((float)(ui.CELL*0.15));
    hint.setOrigin(sf::Vector2f(hint.getRadius(), hint.getRadius()));
    hint.setFillColor(sf::Color(255, 255, 0, 140));

    while (win.isOpen()) {
        // イベント処理（SFML3: pollEvent -> optional<Event>）
        while (true) {
            auto ev = win.pollEvent();
            if (!ev) break;
            auto &e = *ev;

            if (e.is<sf::Event::Closed>()) {
                win.close();
                continue;
            }

            if (e.is<sf::Event::KeyPressed>()) {
                if (auto kp = e.getIf<sf::Event::KeyPressed>()) {
                    if (kp->code == sf::Keyboard::Key::R) {
                        b.assign(N, std::vector<char>(N, EMPTY));
                        b[3][3] = WHITE; b[4][4] = WHITE;
                        b[3][4] = BLACK; b[4][3] = BLACK;
                        turn = BLACK; lastPass = false; gameOver = false;
                        win.setTitle("Reversi (SFML) - Turn: Black");
                        std::cout << "Reset.\n";
                    }
                }
            }

            if (!gameOver && e.is<sf::Event::MouseButtonPressed>()) {
                if (auto mb = e.getIf<sf::Event::MouseButtonPressed>()) {
                    if (mb->button != sf::Mouse::Button::Left) continue;
                    int r, c;
                    if (!ui.posToRC(mb->position, r, c)) continue;

                    if (turn == WHITE && aiWhite) {
                        continue;
                    }

                    if (!isLegal(b, turn, r, c)) {
                        std::cout << "Illegal move at " << (char)('a'+c) << (r+1) << "\n";
                        continue;
                    }

                    applyMove(b, turn, r, c);
                    turn = opponent(turn);
                    win.setTitle(std::string("Reversi (SFML) - Turn: ") + (turn==BLACK ? "Black" : "White"));

                    auto moves = legalMoves(b, turn);
                    if (moves.empty()) {
                        if (lastPass) {
                            gameOver = true;
                            auto [x,o] = countDiscs(b);
                            std::string title = "Game Over - X:" + std::to_string(x) + " O:" + std::to_string(o);
                            win.setTitle(title);
                            std::cout << title << "\n";
                        } else {
                            std::cout << (turn==BLACK ? "Black" : "White") << " has no legal moves -> PASS\n";
                            lastPass = true;
                            turn = opponent(turn);
                            win.setTitle(std::string("Reversi (SFML) - Turn: ") + (turn==BLACK ? "Black" : "White"));
                        }
                    } else {
                        lastPass = false;
                    }
                }
            }
        }

        // AI（白）を動かす場合（フラグON時）
        if (!gameOver && aiWhite && turn == WHITE) {
            Move m = chooseMoveAI(b, WHITE);
            if (m.r == -1 && m.c == -1) {
                if (lastPass) {
                    gameOver = true;
                    auto [x,o] = countDiscs(b);
                    std::string title = "Game Over - X:" + std::to_string(x) + " O:" + std::to_string(o);
                    win.setTitle(title);
                    std::cout << title << "\n";
                } else {
                    std::cout << "White (AI) PASS\n";
                    lastPass = true;
                    turn = BLACK;
                    win.setTitle("Reversi (SFML) - Turn: Black");
                }
            } else {
                applyMove(b, WHITE, m.r, m.c);
                std::cout << "White (AI) move: " << (char)('a'+m.c) << (m.r+1) << "\n";
                turn = BLACK;
                lastPass = false;
                win.setTitle("Reversi (SFML) - Turn: Black");
            }
        }

        // 盤が埋まったら終了
        auto [x,o] = countDiscs(b);
        if (!gameOver && x + o == N*N) {
            gameOver = true;
            std::string title = "Game Over - X:" + std::to_string(x) + " O:" + std::to_string(o);
            win.setTitle(title);
            std::cout << title << "\n";
        }

        // --- 描画 ---
        win.clear(sf::Color(30, 30, 30));
        win.draw(boardBg);
        for (auto &line : gridLines) win.draw(line);

        // 石の描画
        for (int r=0; r<N; ++r) for (int c=0; c<N; ++c) {
            if (b[r][c] == EMPTY) continue;
            sf::Vector2f tl = ui.cellTopLeft(r,c);
            sf::Vector2f center = {tl.x + ui.CELL/2.f, tl.y + ui.CELL/2.f};
            disc.setPosition(center);
            if (b[r][c] == BLACK) disc.setFillColor(sf::Color::Black);
            else disc.setFillColor(sf::Color::White);
            win.draw(disc);
        }

        // 合法手ハイライト（現在の手番）
        if (!gameOver) {
            auto moves = legalMoves(b, turn);
            for (auto m : moves) {
                sf::Vector2f tl = ui.cellTopLeft(m.r, m.c);
                sf::Vector2f center = {tl.x + ui.CELL/2.f, tl.y + ui.CELL/2.f};
                hint.setPosition(center);
                win.draw(hint);
            }
        }

        // ゲームオーバー時の半透明オーバーレイ
        if (gameOver) {
            sf::RectangleShape overlay(sf::Vector2f((float)ui.W, (float)ui.H));
            overlay.setFillColor(sf::Color(0,0,0,90));
            win.draw(overlay);
        }

        win.display();
    }

    return 0;
}
