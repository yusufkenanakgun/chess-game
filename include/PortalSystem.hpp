#pragma once

#include "ConfigReader.hpp"
#include "ChessBoard.hpp"
#include "ChessPiece.hpp"
#include "Portal.hpp"

#include <list>

/**
 * @brief Class responsible for handling portals
 */
class PortalSystem {
public:
    /**
     * @brief Initialize portal system with the given config
     */
    explicit PortalSystem(ChessBoard& board, 
                          const std::vector<PortalConfig>& portal_configs);

    /**
     * @brief Record a portal use
     */
    void startCooldown(Position position);

    /**
     * @brief Decrease cooldowns by one. 
     * Must be called by GameManager each turn
     */
    void decreaseCooldowns();


private:
    ChessBoard& board;
};