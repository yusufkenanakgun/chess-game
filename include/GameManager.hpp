#pragma once

#include "ConfigReader.hpp"
#include "ChessBoard.hpp"
#include "MoveValidator.hpp"
#include "PortalSystem.hpp"

/**
 * @brief Class responsible for handling game state & chess logic.
 */
class GameManager {
public:
    /**
     * @brief Initialize a chess game with the given config
     */
    explicit GameManager(const GameSettings& game_setting, 
                         const std::vector<PieceConfig>& piece_configs, 
                         const std::vector<PortalConfig>& portal_configs);

    /**
     * @brief Play a chess game interactively on the console
     */
    void playInteractively();

    /**
     * @brief Play a single turn
     * @returns true if the movement is valid and the piece was moved
     */
    bool playTurn(Position piece_position, Position destination);
    bool playTurn(ChessPiece& piece, Position destination);

    /**
     * @brief Get a brief description as to why turn was rejected
     */
    std::string getTurnError();

    /**
     * @brief Returns the current player
     */
    team_t getCurrentPlayer();

    /**
     * @brief Check if the king of the given team is under check
     */
    bool isKingUnderCheck(team_t team);

    /**
     * @brief Returns whether the game is over
     */
    bool isGameOver();

    /**
     * @brief Get the amount of turns played
     */
    int getMoveCount();

    /**
     * @brief Get move limit if there is any
     */
    int getMoveLimit();

    /**
     * @brief Returns the winner
     */
    team_t getWinner();

    /**
     * @brief Get the chess board instance
     */
    const ChessBoard& getBoard();

private:
    ChessBoard board;
    MoveValidator validator;
    PortalSystem portal_system;

    void checkGameOver();
    bool withTurnError(std::string err);
    std::string turn_error;
    const ChessPiece* checking_piece;
    
    team_t winner;
    team_t current_player;
    bool game_over;
    int move_count;
    int move_limit;
};