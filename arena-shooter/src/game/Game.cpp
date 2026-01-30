#include "Game.h"
#include "raylib.h"

namespace arena {

static Game* s_instance = nullptr;

Game::Game()
    : m_state(GameState::MENU)
    , m_accumulator(0.0f)
    , m_tickCount(0)
{
    s_instance = this;
}

Game::~Game()
{
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

Game& Game::instance()
{
    return *s_instance;
}

void Game::init()
{
    // Initialize map
    m_map = std::make_unique<Map>();
    m_map->generateDefaultArena();

    // Create local player
    m_localPlayer = std::make_unique<Player>(0, "Player");

    // Set initial spawn position
    const SpawnPoint& spawn = m_map->getRandomSpawnPoint();
    m_localPlayer->setPosition(spawn.position);
    m_localPlayer->getState().yaw = spawn.rotation;

    m_state = GameState::PLAYING;
}

void Game::update(float deltaTime)
{
    if (m_state != GameState::PLAYING) {
        return;
    }

    // Fixed timestep for game logic
    m_accumulator += deltaTime;

    while (m_accumulator >= TICK_INTERVAL) {
        // Update local player
        if (m_localPlayer) {
            m_localPlayer->update(TICK_INTERVAL);
        }

        // Update remote players
        for (auto& player : m_players) {
            if (player) {
                player->update(TICK_INTERVAL);
            }
        }

        // Update projectiles
        for (auto& proj : m_projectiles) {
            if (proj.active) {
                proj.position.x += proj.velocity.x * TICK_INTERVAL;
                proj.position.y += proj.velocity.y * TICK_INTERVAL;
                proj.position.z += proj.velocity.z * TICK_INTERVAL;
                proj.lifetime -= TICK_INTERVAL;
                if (proj.lifetime <= 0.0f) {
                    proj.active = false;
                }
            }
        }

        m_accumulator -= TICK_INTERVAL;
        m_tickCount++;
    }
}

void Game::render()
{
    // Rendering is handled by Renderer class
    // This method exists for potential game-specific render logic
}

void Game::shutdown()
{
    m_players.clear();
    m_localPlayer.reset();
    m_map.reset();
    m_projectiles.clear();
    m_state = GameState::MENU;
}

} // namespace arena
