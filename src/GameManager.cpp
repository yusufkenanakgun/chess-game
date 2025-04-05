#include "GameManager.hpp"

GameManager::GameManager(const GameSettings& game_setting, 
                         const std::vector<PieceConfig>& piece_configs, 
                         const std::vector<PortalConfig>& portal_configs)
                         : board(game_setting, piece_configs) 
                         , validator(board, piece_configs)
                         , portal_system(board, portal_configs) {
    current_player = WHITE;
    game_over = false;
    move_count = 0;
    checking_piece = nullptr;
    move_limit = game_setting.turn_limit * 2;
}

bool GameManager::isGameOver() {
    return game_over;
}

int GameManager::getMoveCount() {
    return move_count;
}

team_t GameManager::getCurrentPlayer() {
    return current_player;
}

bool GameManager::withTurnError(std::string err) {
    turn_error = err;
    return false;
}

std::string GameManager::getTurnError() {
    return turn_error;
}

const ChessBoard& GameManager::getBoard() {
    return board;
}

team_t GameManager::getWinner() {
    return winner;
}

int GameManager::getMoveLimit() {
    return move_limit;
}

void GameManager::checkGameOver() {
    bool still_check = isKingUnderCheck(current_player);
    bool move_check = true;

    // Check all possible moves by current player
    std::list<ChessPiece*> team_pieces = board.getPiecesOfTeam(current_player);
    for (ChessPiece* piece : team_pieces) {
        Position og_pos = piece->position;
        std::set<Position> moves = validator.getPossibleMoves(*piece);

        for (Position move : moves) {
            ChessPiece* opponent_piece = board.getPieceAtPosition(move);
            ChessPiece op_copy("", false, Position(0,0), WHITE); // Dummy value

            if (opponent_piece != nullptr) {
                op_copy = *opponent_piece;
                board.removePiece(opponent_piece);
            }

            bool og_used = piece->used;
            board.movePiece(*piece, move);
            bool check = isKingUnderCheck(current_player);

            // Rewind the piece
            board.movePiece(*piece, og_pos);
            piece->used = og_used;
            if (opponent_piece != nullptr)
                board.addPiece(op_copy);

            // If legal move is found, break.
            if (!check) {
                move_check = false;
                break;
            }
        }
    }

    // If no legal move can be made, game is over.
    if (move_check) {
        game_over = true;
        if (!still_check) {
            winner = TIE;
        } else {
            winner = current_player == WHITE ? BLACK : WHITE;
        }
    } 
    else if (move_limit > 0 && move_count >= move_limit) {
        game_over = true;
        winner = TIE;
    }
}

bool GameManager::isKingUnderCheck(team_t team) {
    const ChessPiece* king = board.getKingOfTeam(team);

    if (king == nullptr)
        throw std::runtime_error("There is no king, which is impossible.");

    for (const ChessPiece& piece : board.getPieces()) {
        if (validator.validateMove(piece, king->position)) {
            checking_piece = &piece;
            //std::cout << "king under check by " << checking_piece->type << std::endl;
            //std::cout << "king under check from " << piece.position << "to" << king->position << std::endl;
            return true;
        }
    }

    return false;
}

bool GameManager::playTurn(Position piece_position, Position destination) {
    ChessPiece* piece = board.getPieceAtPosition(piece_position);
    if (piece == nullptr) 
        return withTurnError("No Piece at Position");

    return playTurn(*piece, destination);
}

bool GameManager::playTurn(ChessPiece& piece, Position destination) {
    if (isGameOver()) 
        return withTurnError("Game is Over");
    
    if (!validator.validateMove(piece, destination))
        return withTurnError("Invalid Move");
    
    if (piece.team != current_player)
        return withTurnError("Wrong Player");

    Position original_position = piece.position;

    Portal* portal = board.getPortalAtPosition(destination);
    if (portal != nullptr) {
        destination = portal->exit == destination ? portal->entry : portal->exit; // which way

        if (!validator.validatePortalUse(piece, *portal))
            return withTurnError("Portal is Unusable");
    }

    ChessPiece* opponent_piece = board.getPieceAtPosition(destination);
    ChessPiece op_copy("", false, Position(0,0), WHITE); // Dummy value

    if (opponent_piece != nullptr) {
        if (opponent_piece->team == current_player) 
            throw std::runtime_error("MoveValidator must have catched this");

        op_copy = *opponent_piece;
        board.removePiece(opponent_piece);
    }

    board.movePiece(piece, destination);

    if (isKingUnderCheck(current_player)) { 
        // Illegal move, rewind
        board.movePiece(piece, original_position);
        if (opponent_piece != nullptr) {
            board.addPiece(op_copy);
        }
        return withTurnError("King Under Check! Reversed");
    }

    portal_system.decreaseCooldowns();

    if (portal != nullptr)
        portal_system.startCooldown(portal->entry);

    current_player = current_player == WHITE ? BLACK : WHITE;
    move_count++;
    checkGameOver();
    return true;
}

void GameManager::playInteractively() {
    bool was_valid = true;

    while (!isGameOver()) {
        std::set<Position> team_positions = board.getPositionsOfTeam(current_player);
        board.printBoard(team_positions);
        std::cout << std::endl;

        if (!was_valid) {
            // Last error message
            std::cout << "=== " << turn_error << " ===" << std::endl;
            std::cout << std::endl;
        }

        if (isKingUnderCheck(current_player)) {
            std::cout << "=== " << "King is Under Check!" << " ===" << std::endl;
            std::cout << std::endl;
        }

        std::cout << "=== Move " << move_count + 1 << " ===" << std::endl;
        std::cout << "Turn: " << (current_player == WHITE ? "White" : "Black") << std::endl;
        
        unsigned char x;
        int y;
        std::cout << "Select piece (cN): ";
        std::cin >> x;
        std::cin >> y;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            turn_error = "Invalid Input";
            was_valid = false;
            continue;
        }

        ChessPiece* piece = board.getPieceAtPosition(Position(x - 'a', y - 1));

        if (piece == nullptr) {
            turn_error = "No Piece at Position";
            was_valid = false;
            continue;
        }

        if (piece->team != current_player) {
            turn_error = "That's Opponent's Piece";
            was_valid = false;
            continue;
        }

        std::set<Position> moves = validator.getPossibleMoves(*piece);
        std::set<Position> highlight = moves;
        highlight.insert(piece->position);

        std::cout << std::endl;
        board.printBoard(highlight);
        std::cout << std::endl;

        std::cout << "=== Selected " << piece->type << " ===" << std::endl;

        std::cout << "Possible Moves: ";
        for (const Position& move : moves) 
            std::cout << move << " ";
        std::cout << std::endl;

        unsigned char dx;
        int dy;
        std::cout << "Select destination (cN): ";
        std::cin >> dx;
        std::cin >> dy;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            turn_error = "Invalid Input";
            was_valid = false;
            continue;
        }
        
        std::cout << std::endl;

        was_valid = playTurn(*piece, Position(dx - 'a', dy - 1));
    }

    if (checking_piece == nullptr)
        throw std::runtime_error("Checking piece was not set");
        
    ChessPiece* king_piece = board.getKingOfTeam(current_player);
    board.printBoard(std::set<Position>{checking_piece->position, king_piece->position});
    std::cout << std::endl;

    std::cout << "=== Game Finished ===" << std::endl;
    if (winner == TIE)
        std::cout << "No winner: Stalemate" << std::endl; 
    else
        std::cout << "Winner: " << (winner == WHITE ? "White" : "Black") << std::endl; 
    std::cout << "Turns played: " << move_count / 2 << std::endl;
}