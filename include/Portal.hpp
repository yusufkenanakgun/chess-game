#pragma once

#include "ConfigReader.hpp"

/**
 * @brief Struct representing a portal
 */
struct Portal {
    /**
     * @brief ID of this portal
     */
    std::string id;

    /**
     * @brief Entry point of portal
     */
    Position entry;

    /**
     * @brief Exit point of portal
     */
    Position exit;

    /**
     * @brief Whether the portal is bidirectional
     */
    bool both_ways;

    /**
     * @brief Is white color allowed entry
     */
    bool white_allowed;

    /**
     * @brief Is black color allowed entry
     */
    bool black_allowed;

    /** 
     * @brief Get cooldown in turns
     */
    int cooldown;

    /**
     * @brief
     */
    int current_cooldown;

    /**
     * @brief Initialize a portal with the given config
     */
    inline Portal(std::string id, Position entry, Position exit, bool both_ways,
                  bool white_allowed, bool black_allowed, int cooldown)
        : id(id), entry(entry), exit(exit), both_ways(both_ways), white_allowed(white_allowed)
        , black_allowed(black_allowed), cooldown(cooldown), current_cooldown(0) { }
};