#include "GameManager.hpp"

MoveValidator::MoveValidator(const ChessBoard& board,
                             const std::vector<PieceConfig>& piece_configs) 
                             : board(board) {
    for(auto &piece_config : piece_configs) {
        rules[piece_config.type] = piece_config.movement;
    }
}

#define PUSH_IF_VALID(p,x) if (validateMove(p, x)) moves.insert(x);
std::set<Position> MoveValidator::getPossibleMoves(const ChessPiece& piece) {
    std::set<Position> moves;
    Position origin = piece.position;
    MovementRules rule = rules[piece.type];

    if (rule.forward == -1) {
        for (int i = 0; i < board.getSize(); i++)
            PUSH_IF_VALID(piece, Position(origin.x, i)); // Vertical, same as BW
    } else {
        PUSH_IF_VALID(piece, Position(origin.x, origin.y + rule.forward)); // White
        PUSH_IF_VALID(piece, Position(origin.x, origin.y - rule.forward)); // Black
    }

    if (rule.first_move_forward == -1) {
        for (int i = 0; i < board.getSize(); i++)
            PUSH_IF_VALID(piece, Position(origin.x, i)); // Vertical, same as FW
    } else {
        PUSH_IF_VALID(piece, Position(origin.x, origin.y + rule.first_move_forward)); // White
        PUSH_IF_VALID(piece, Position(origin.x, origin.y - rule.first_move_forward)); // Black
    }

    if (rule.backward == -1) {
        for (int i = 0; i < board.getSize(); i++)
            PUSH_IF_VALID(piece, Position(origin.x, i)); // Vertical, same as FW
    } else {
        PUSH_IF_VALID(piece, Position(origin.x, origin.y - rule.backward)); // White
        PUSH_IF_VALID(piece, Position(origin.x, origin.y + rule.backward)); // Black
    }

    if (rule.sideways == -1) {
        for (int i = 0; i < board.getSize(); i++)
            PUSH_IF_VALID(piece, Position(i, origin.y)); // Horizontal
    } else {
        PUSH_IF_VALID(piece, Position(origin.x + rule.sideways, origin.y)); // Both
        PUSH_IF_VALID(piece, Position(origin.x - rule.sideways, origin.y)); // Both
    }

    if (rule.diagonal == -1) {
        for (int i = 0; i < board.getSize(); i++) {
            PUSH_IF_VALID(piece, Position(origin.x - origin.y + i, i)); // R Diagonal
            PUSH_IF_VALID(piece, Position(origin.x + origin.y - i, i)); // L Diagonal

        }
    } else {
        PUSH_IF_VALID(piece, Position(origin.x + rule.diagonal, origin.y + rule.diagonal));
        PUSH_IF_VALID(piece, Position(origin.x + rule.diagonal, origin.y - rule.diagonal));
        PUSH_IF_VALID(piece, Position(origin.x - rule.diagonal, origin.y + rule.diagonal));
        PUSH_IF_VALID(piece, Position(origin.x - rule.diagonal, origin.y - rule.diagonal));
    }

    if (rule.diagonal_capture == -1) {
        for (int i = 0; i < board.getSize(); i++) {
            PUSH_IF_VALID(piece, Position(origin.x - origin.y + i, i)); // R Diagonal
            PUSH_IF_VALID(piece, Position(origin.x + origin.y - i, i)); // L Diagonal

        }
    } else {
        PUSH_IF_VALID(piece, Position(origin.x + rule.diagonal_capture, origin.y + rule.diagonal_capture));
        PUSH_IF_VALID(piece, Position(origin.x + rule.diagonal_capture, origin.y - rule.diagonal_capture));
        PUSH_IF_VALID(piece, Position(origin.x - rule.diagonal_capture, origin.y + rule.diagonal_capture));
        PUSH_IF_VALID(piece, Position(origin.x - rule.diagonal_capture, origin.y - rule.diagonal_capture));
    }

    if (rule.l_shape) {
        PUSH_IF_VALID(piece, Position(origin.x + 2, origin.y + 1));
        PUSH_IF_VALID(piece, Position(origin.x + 2, origin.y - 1));
        PUSH_IF_VALID(piece, Position(origin.x - 2, origin.y + 1));
        PUSH_IF_VALID(piece, Position(origin.x - 2, origin.y - 1));
        PUSH_IF_VALID(piece, Position(origin.x + 1, origin.y + 2));
        PUSH_IF_VALID(piece, Position(origin.x + 1, origin.y - 2));
        PUSH_IF_VALID(piece, Position(origin.x - 1, origin.y + 2));
        PUSH_IF_VALID(piece, Position(origin.x - 1, origin.y - 2));
    }

    return moves;
}
#undef PUSH_IF_VALID

bool MoveValidator::validateMove(const ChessPiece& piece, Position destination) {
    // Check 0: Out of bounds
    if (destination.x >= board.getSize() || destination.y >= board.getSize()
        || destination.x < 0 || destination.y < 0)
        return false;

    const ChessPiece* opponent = board.getPieceAtPosition(destination);

    // Check 1: Validate opponent
    if (opponent != nullptr) {
        if (opponent->team == piece.team)
            return false;
    }

    // Check 2: Validate path
    Position origin = piece.position;
    MovementRules rule = rules[piece.type];

    int dx = destination.x - origin.x;
    int dy = destination.y - origin.y;

    if (piece.team == BLACK) dy = -dy;

    if (dx == 0 && dy == 0) {
        // No movement
        return false;
    }
    else if (abs(dx) == abs(dy)) {
        // Diagonal
        bool valid = false;
        if (rule.diagonal == -1 || rule.diagonal == abs(dx))
            valid = true;
        if (opponent != nullptr && dy > 0 // Diagonal cap. requires forward
            && (rule.diagonal_capture == -1 || rule.diagonal_capture == abs(dx)))
            valid = true;
        if (!valid) return false;
    }
    else if (dy == 0) {
        // Horizontal
        if (rule.sideways != -1 && rule.sideways != abs(dx))
            return false;
    }
    else if (dx == 0 && dy > 0) {
        // Forward
        if (piece.type == "pawn" && opponent != nullptr)
            return false;
            
        bool valid = false;
        if (rule.forward == -1 || rule.forward == dy)
            valid = true;
        if (!piece.used && (rule.first_move_forward == -1 || rule.first_move_forward == dy))
            valid = true;
        if (!valid) return false;
    }
    else if (dx == 0 && dy < 0) {
        // Backward
        if (rule.backward != -1 && rule.backward != -dy)
            return false;
    }
    else if ((abs(dx) == 1 && abs(dy) == 2) || (abs(dx) == 2 && abs(dy) == 1)) {
        // L-shape
        if (rule.l_shape)
            return true;
        else
            return false;
    }
    else {
        // Invalid
        return false;
    }

    // Check 3: Obstacle on path
    int range = std::max(std::abs(dx), std::abs(dy));
    for (int i = 1; i < range; i++) {
        int x = (destination.x - origin.x) / range * i + origin.x;
        int y = (destination.y - origin.y) / range * i + origin.y;
        if (board.getPieceAtPosition(Position(x, y)) != nullptr)
            return false;
    }
    
    return true;
}

bool MoveValidator::validatePortalUse(const ChessPiece& piece, Portal& portal) {
    if (portal.current_cooldown == 0 && 
        ((portal.black_allowed && piece.team == BLACK) || (portal.white_allowed && piece.team == WHITE)))
        return true;
    else
        return false;
}