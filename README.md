# CSE 211 - Chess Project
This project implements a chess game with optional fantasy elements.

## Building

### Requirements
1. A modern C++ compiler (GCC, Clang)
2. GNU Make
3. Internet connection (for JSON and Unity dependencies)

### Build Steps
1. Install dependencies using `make deps`.
2. Build the project with `make`. 
3. Run with `./bin/chess_game <config_file>` or `make run`.

## Configuration
You can create custom chess game configurations by creating
JSON files that describe the game. You may inspect the
existing configuration files under `data/` to help understand the format.

### Example Games
1. `./bin/chess_game data/chess_pieces.json` Normal chess.
2. `./bin/chess_game data/chess_nopawn.json` Chess with no pawns.
3. `./bin/chess_game data/fantasy_chess.json` Chess with portals.
4. `./bin/chess_game data/fantasy_color.json` Chess with portals limited to colors.
5. `./bin/chess_game data/chess_limit.json` Chess with no pawns & 4 turn limit.

## Unit Testing
1. Install dependencies using `make deps`.
2. Run with `./bin/chess_test -v` or `make test`.
