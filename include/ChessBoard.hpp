#pragma once

#include "ConfigReader.hpp"
#include "ChessPiece.hpp"
#include "Portal.hpp"

#include <list>
#include <set>

/**
 * @brief Class representing a chess board
 */
class ChessBoard {
public:
    /**
     * @brief Initialize a chess board with the given config
     */
    explicit ChessBoard(const GameSettings& game_setting, 
                        const std::vector<PieceConfig>& piece_configs);

    /**
     * @brief Get board length
     */
    int getSize() const;

    /**
     * @brief Get chess pieces
     */
    const std::list<ChessPiece>& getPieces() const;
    std::list<ChessPiece>& getPieces();

    /**
     * @brief Get portals
     */
    const std::list<Portal>& getPortals() const;
    std::list<Portal>& getPortals();

    /**
     * @brief Get the chess pieces of the given team
     */
    std::list<const ChessPiece*> getPiecesOfTeam(team_t team) const;
    std::list<ChessPiece*> getPiecesOfTeam(team_t team);

    /**
     * @brief Get king piece of the given team
     */
    ChessPiece* getKingOfTeam(team_t team);

    /**
     * @brief Get piece positions of team
     */
    std::set<Position> getPositionsOfTeam(team_t team) const;

    /**
     * @brief Remove chess piece
     */
    void removePiece(const ChessPiece* piece);

    /**
     * @brief Add chess piece
     */
    void addPiece(const ChessPiece& piece);

    /**
     * @brief Add portal
     */
    void addPortal(const Portal& portal);

    /**
     * @brief Exchange positions of chess pieces
     */
    void exchangePiecePositions(ChessPiece& piece, ChessPiece& other);

    /**
     * @brief Get the chess piece at the given location
     */
    ChessPiece* getPieceAtPosition(Position position);
    const ChessPiece* getPieceAtPosition(Position position) const;

    /**
     * @brief Get the portal at the given location
     */
    Portal* getPortalAtPosition(Position position);

    /**
     * @brief Teleport a piece into location
     */
    void movePiece(ChessPiece& piece, Position location);

    /**
     * @brief Print the board in a human readable format
     */
    void printBoard(std::set<Position> highlight) const;
    void printBoard() const;

private:
    /**
     * @brief Remaining chess pieces
     */
    std::list<ChessPiece> pieces;

    /**
     * @brief Portals
     */
    std::list<Portal> portals;

    /**
     * @brief Board length
     */
    int size;
};