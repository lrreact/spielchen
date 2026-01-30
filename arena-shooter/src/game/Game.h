#pragma once

#include <vector>
#include <memory>

#include "Player.h"
#include "Map.h"
#include "Projectile.h"

namespace arena {

// Game constants (RESPAWN_TIME is in Player.h)
constexpr int GAME_MAX_PLAYERS = 16;
constexpr float GAME_TICK_RATE = 64.0f;
constexpr float GAME_TICK_INTERVAL = 1.0f / GAME_TICK_RATE;

enum class GameState {
    MENU,
    CONNECTING,
    PLAYING,
    PAUSED,
    DISCONNECTED
};

class Game {
public:
    Game();
    ~Game();

    // Core game loop
    void init();
    void update(float deltaTime);
    void render();
    void shutdown();

    // State management
    GameState getState() const { return m_state; }
    void setState(GameState state) { m_state = state; }

    // Player management
    Player* getLocalPlayer() { return m_localPlayer.get(); }
    const std::vector<std::unique_ptr<Player>>& getPlayers() const { return m_players; }

    // Map access
    Map* getMap() { return m_map.get(); }

    // Singleton access (for raylib callbacks)
    static Game& instance();

private:
    GameState m_state;
    std::unique_ptr<Player> m_localPlayer;
    std::vector<std::unique_ptr<Player>> m_players;
    std::unique_ptr<Map> m_map;
    std::vector<Projectile> m_projectiles;

    float m_accumulator; // For fixed timestep
    uint32_t m_tickCount;
};

} // namespace arena
