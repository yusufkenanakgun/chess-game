#include "PortalSystem.hpp"

#include <iostream>
#include <exception>
#include <algorithm>

PortalSystem::PortalSystem(ChessBoard& board,
                           const std::vector<PortalConfig>& portal_configs)
                           : board(board) {
    
    for (const PortalConfig& portal_config : portal_configs) {
        auto &ac = portal_config.properties.allowed_colors;
        Portal portal(portal_config.id,
                      portal_config.positions.entry,
                      portal_config.positions.exit, 
                      !portal_config.properties.preserve_direction,
                      std::find(ac.begin(), ac.end(), "white") != ac.end(), 
                      std::find(ac.begin(), ac.end(), "black") != ac.end(), 
                      portal_config.properties.cooldown);
        board.addPortal(portal);
    }
}

void PortalSystem::startCooldown(Position position) {
    Portal* portal = board.getPortalAtPosition(position);
    if (portal == nullptr)
        throw std::runtime_error("startCooldown called on null portal");

    portal->current_cooldown = portal->cooldown;
}

void PortalSystem::decreaseCooldowns() {
    for (Portal& portal : board.getPortals()) {
        if (portal.current_cooldown > 0) portal.current_cooldown--;
    }
}