# famous-game
- Console Reversi (C++)
- SFML Reversi (C++)

## Build (macOS)
```bash
clang++ -std=c++17 -O2 reversi.cpp -o reversi && ./reversi
clang++ -std=c++17 -O2 reversi_sfml.cpp -o reversi_sfml \
  -lsfml-graphics -lsfml-window -lsfml-system && ./reversi_sfml