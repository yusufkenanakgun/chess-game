#pragma once

#include "ConfigReader.hpp"

/** @brief Struct representing a chess piece */
struct ChessPiece {
    /**
     * @brief Position within the chess board
     */
    Position position;

    /**
     * @brief The type of the piece
     */
    std::string type;

    /**
     * @brief Whether type is king
     */
    bool king_type;

    /**
     * @brief The piece is white
     */
    team_t team;

    /**
     * @brief Whether the piece was used at all
     */
    bool used;

    /**
     * @brief Initialize a chess piece with given values
     */
    inline ChessPiece(std::string type, bool king_type, Position position, team_t team)
        : position(position), type(type), king_type(king_type), team(team), used(false) { }

    /**
     * @brief Initialize a chess piece with given values
     */
    inline ChessPiece(std::string type, bool king_type, Position position, team_t team, bool used)
        : position(position), type(type), king_type(king_type), team(team), used(used) { }
};