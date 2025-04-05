#include "ChessBoard.hpp"

#include <iostream>
#include <exception>

ChessBoard::ChessBoard(const GameSettings& game_setting, 
                       const std::vector<PieceConfig>& piece_configs)
                       : size(0) {
    // Set properties with help from game settings
    this->size = game_setting.board_size;

    // Initialize each piece with help from piece config
    for (const auto& piece_config : piece_configs) {
        for (int i = 0; i < piece_config.count; i++) {
            Position black_pos = piece_config.black_positions[i];
            Position white_pos = piece_config.white_positions[i];

            ChessPiece black_piece(piece_config.type, piece_config.king_type, black_pos, BLACK);
            ChessPiece white_piece(piece_config.type, piece_config.king_type, white_pos, WHITE);

            addPiece(black_piece);
            addPiece(white_piece);
        }
    }
}

int ChessBoard::getSize() const {
    return this->size;
}

const std::list<ChessPiece>& ChessBoard::getPieces() const {
    return this->pieces;
}

std::list<ChessPiece>& ChessBoard::getPieces() {
    return this->pieces;
}

std::list<Portal>& ChessBoard::getPortals() {
    return this->portals;
}

const std::list<Portal>& ChessBoard::getPortals() const {
    return this->portals;
}

std::list<const ChessPiece*> ChessBoard::getPiecesOfTeam(team_t team) const {
    std::list<const ChessPiece*> team_pieces;

    for (const ChessPiece& piece : pieces)
        if (piece.team == team)
            team_pieces.push_back(&piece);

    return team_pieces;
}

std::list<ChessPiece*> ChessBoard::getPiecesOfTeam(team_t team) {
    std::list<ChessPiece*> team_pieces;

    for (ChessPiece& piece : pieces)
        if (piece.team == team)
            team_pieces.push_back(&piece);

    return team_pieces;
}

ChessPiece* ChessBoard::getKingOfTeam(team_t team) {
    for (ChessPiece& piece : getPieces())
        if (piece.king_type && piece.team == team)
            return &piece;

    return nullptr;
}

ChessPiece* ChessBoard::getPieceAtPosition(Position position) {
    for(auto& piece : pieces) {
        if (piece.position == position) return &piece;
    }

    return nullptr;
}

Portal* ChessBoard::getPortalAtPosition(Position position) {
    for(auto& piece : portals) {
        if (piece.entry == position 
        || (piece.both_ways && piece.exit == position)) return &piece;
    }

    return nullptr;
}

const ChessPiece* ChessBoard::getPieceAtPosition(Position position) const {
    for(auto& piece : pieces) {
        if (piece.position == position) return &piece;
    }

    return nullptr;
}

std::set<Position> ChessBoard::getPositionsOfTeam(team_t team) const {
    std::set<Position> positions;

    for (const ChessPiece& piece : pieces)
        if (piece.team == team)
            positions.insert(piece.position);

    return positions;
}

void ChessBoard::removePiece(const ChessPiece* piece) {
    bool removed = false;
    for (auto it = pieces.begin(); it != pieces.end();) {
        if (&(*it) == piece) {
            it = pieces.erase(it); // Erase and get the next valid iterator
            removed = true;
        } else {
            ++it; // Increment if no removal
        }
    }

    if (!removed)
        throw std::runtime_error("Given chess piece was not found.");
}

void ChessBoard::addPiece(const ChessPiece& piece) {
    if (getPieceAtPosition(piece.position) != nullptr) 
        throw std::runtime_error("There is a chess piece at the destination.");
    
    this->pieces.push_back(piece);
}

void ChessBoard::addPortal(const Portal& portal) {
    if (getPortalAtPosition(portal.entry) != nullptr) 
        throw std::runtime_error("There is a portal at the destination.");
    
    this->portals.push_back(portal);
}

void ChessBoard::movePiece(ChessPiece& piece, Position destination) {
    if (getPieceAtPosition(destination) != nullptr) 
        throw std::runtime_error("There is a chess piece at the destination.");

    piece.used = true;
    piece.position = destination;
}

void ChessBoard::exchangePiecePositions(ChessPiece& piece, ChessPiece& other) {
    Position temp = piece.position;
    piece.position = other.position;
    other.position = temp;
}

void ChessBoard::printBoard(std::set<Position> highlight) const {
    // Create a 2D array to represent the board
    std::vector<std::vector<std::string>> board(this->size, std::vector<std::string>(this->size, "   "));

    // Populate the board with portals
    for (const auto& portal : this->portals) {
        const auto& pos = portal.entry;
        const auto& pos2 = portal.exit;

        // Portal under cooldown
        if (portal.current_cooldown != 0) {
            board[pos.y][pos.x] = std::string(3, '0' + portal.current_cooldown);
            board[pos.y][pos.x][0] = ' ';
            board[pos.y][pos.x][2] = ' ';

            board[pos2.y][pos2.x] = std::string(3, '0' + portal.current_cooldown);
            board[pos2.y][pos2.x][0] = ' ';
            board[pos2.y][pos2.x][2] = ' ';
        } else {
            board[pos.y][pos.x] = std::string(3, portal.id[0]); // Assign symbol to portal
            board[pos.y][pos.x][0] = portal.both_ways ? '=' : '>';
            board[pos.y][pos.x][2] = portal.both_ways ? '=' : '<';
            
            board[pos2.y][pos2.x] = std::string(3, portal.id[0]); // Assign symbol to portal
            board[pos2.y][pos2.x][0] = portal.both_ways ? '=' : '<';
            board[pos2.y][pos2.x][2] = portal.both_ways ? '=' : '>';
        }
    }

    // Populate the board with pieces
    for (const auto& piece : this->pieces) {
        const auto& pos = piece.position;
        char piece_symbol = piece.type[0]; // Get the first letter of the piece type
        board[pos.y][pos.x] = std::string(3, piece_symbol); // Assign symbol to piece
        if (piece.team == WHITE) {
            board[pos.y][pos.x][0] = '^';
            board[pos.y][pos.x][2] = '^';
        } else {
            board[pos.y][pos.x][0] = '.';
            board[pos.y][pos.x][2] = '.';
        }
    }

    // Print the board
    std::cout << "   ";
    for (int x = 0; x < this->size; x++) {
        std::cout << " " << (char) ('a' + x) << "  "; // Column headers
    }
    std::cout << "\n";

    for (int y = this->size - 1; y >= 0; y--) {
        std::cout << "  "; 
        for (int x = 0; x < this->size; x++) {
            Position pos1(x, y);
            Position pos2(x, y + 1);
            if (highlight.find(pos1) != highlight.end() || highlight.find(pos2) != highlight.end())
                std::cout << "+==="; // Highlighted border
            else
                std::cout << "+---"; // Horizontal borders
        }
        std::cout << "+\n";

        std::cout << y + 1 << " "; // Row header
        for (int x = 0; x < this->size; x++) {
            Position pos1(x - 1, y);
            Position pos2(x, y);
            if (highlight.find(pos1) != highlight.end() || highlight.find(pos2) != highlight.end())
                std::cout << "I" << board[y][x]; // Highligthed cell
            else
                std::cout << "|" << board[y][x]; // Cell content
        }

        if (highlight.find(Position(this->size - 1, y)) != highlight.end())
            std::cout << "I\n";
        else
            std::cout << "|\n";
    }

    // Print the bottom border
    std::cout << "  "; 
    for (int x = 0; x < this->size; x++) {
        Position pos(x, 0);
        if (highlight.find(pos) != highlight.end())
            std::cout << "+===";
        else
            std::cout << "+---";
    }
    std::cout << "+\n";
}

void ChessBoard::printBoard() const {
    printBoard(std::set<Position>());
}