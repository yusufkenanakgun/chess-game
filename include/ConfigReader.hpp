#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <iostream>

#define WHITE   0
#define BLACK   1
#define TIE     2

/**
 * @brief Opponent type, white (0) or black (1).
 */
typedef unsigned char team_t;

/**
 * @brief Structure to hold position coordinates
 */
struct Position {
  short x;
  short y;

  /**
   * @brief Equality implementation
   */
  inline bool operator==(const Position& other) const {
    return x == other.x && y == other.y;
  }

  /**
   * @brief Lesser implementation
   * Required for <set>
   */
  inline bool operator<(const Position& other) const {
    long itotal = x * std::numeric_limits<short>::max() + y;
    long ttotal = other.x * std::numeric_limits<short>::max() + other.y;
    return itotal < ttotal;
  }

  /**
   * @brief Print implementation
   */
  friend inline std::ostream& operator<<(std::ostream& os, const Position& pos) {
    os << (char)('a' + pos.x);
    os << pos.y + 1;
    return os;
  }

  /**
   * @brief Initializer
   */
  inline Position(int x, int y) : x(x), y(y) { }
  
  /**
   * @brief Default initializer
   */
  inline Position() : x(0), y(0) { }
};

/**
 * @brief Structure to hold movement rules for a piece
 */
struct MovementRules {
  int forward{0};             // Number of squares forward
  int backward{0};            // Number of squares backward
  int sideways{0};            // Number of squares sideways
  int diagonal{0};            // Number of squares diagonally
  bool l_shape{false};        // Knight's L-shaped movement
  int first_move_forward{0};  // Special first move for pawns
  int diagonal_capture{0};    // Diagonal capture for pawns
};

/**
 * @brief Structure to hold piece configuration
 */
struct PieceConfig {
  std::string type;                       // Type of the piece
  bool king_type;                         // Whether type is of king
  std::vector<Position> white_positions;  // Starting positions for white pieces
  std::vector<Position> black_positions;  // Starting positions for black pieces
  MovementRules movement;                 // Movement rules for the piece
  int count;                              // Number of pieces of this type
};

/**
 * @brief Structure to hold game settings
 */
struct GameSettings {
  std::string name;
  int board_size;
  int turn_limit;
};

/**
 * @brief Structure to hold portal positions
 */
struct PortalPositions {
  Position entry;
  Position exit;
};

/**
 * @brief Structure to hold portal properties
 */
struct PortalProperties {
  bool preserve_direction{true};
  std::vector<std::string> allowed_colors;
  int cooldown{0};
};

/**
 * @brief Structure to hold portal configuration
 */
struct PortalConfig {
  std::string id;
  PortalPositions positions;
  PortalProperties properties;
};

/**
 * @brief Class responsible for reading and parsing chess configuration files
 */
class ConfigReader {
 public:
  /**
   * @brief Constructor
   * @param config_path Path to the configuration file
   */
  explicit ConfigReader(const std::string& config_path);

  /**
   * @brief Read and parse the configuration file
   * @return true if successful, false otherwise
   */
  bool readConfig();

  /**
   * @brief Get the parsed game settings
   * @return GameSettings structure containing game configuration
   */
  GameSettings getGameSettings() const;

  /**
   * @brief Get the parsed piece configurations
   * @return Vector of PieceConfig structures
   */
  std::vector<PieceConfig> getPieceConfigs() const;

  /**
   * @brief Get the parsed portal configurations
   * @return Vector of PortalConfig structures
   */
  std::vector<PortalConfig> getPortalConfigs() const;

 private:
  std::string config_path_;
  GameSettings game_settings_;
  std::vector<PieceConfig> piece_configs_;
  std::vector<PortalConfig> portal_configs_;

  /**
   * @brief Parse game settings from JSON
   * @param json JSON object containing game settings
   */
  void parseGameSettings(const nlohmann::json& json);

  /**
   * @brief Parse piece configurations from JSON
   * @param json JSON object containing piece configurations
   */
  void parsePieceConfigs(const nlohmann::json& json);

  /**
   * @brief Parse portal configurations from JSON
   * @param json JSON object containing portal configurations
   */
  void parsePortalConfigs(const nlohmann::json& json);
};
