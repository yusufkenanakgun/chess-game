#pragma once

#include "ConfigReader.hpp"

#include <set>

/**
 * @brief Class responsible for validating moves & getting valid moves
 */
class MoveValidator {
public:
    /**
     * @brief Initialize a move validator with the given config
     */
    explicit MoveValidator(const ChessBoard& board, 
                           const std::vector<PieceConfig>& piece_configs);

    /**
     * @brief Validate a move
     * @returns Whether the move is valid
     */
    bool validateMove(const ChessPiece& piece, Position destionation);

    /**
     * @brief Validate portal usage
     * @returns Whether portal use is valid
     */
    bool validatePortalUse(const ChessPiece& piece, Portal& portal);

    /**
     * @brief Get all the possible moves of a piece
     */
    std::set<Position> getPossibleMoves(const ChessPiece& piece);

private:
    const ChessBoard& board;
    std::map<std::string, MovementRules> rules;
};