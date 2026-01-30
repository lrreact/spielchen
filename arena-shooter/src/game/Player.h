#pragma once

#include "raylib.h"
#include "raymath.h"
#include "Weapon.h"
#include <string>
#include <array>
#include <memory>
#include <cstdint>

namespace arena {

// Player constants
constexpr float PLAYER_HEIGHT = 1.8f;
constexpr float PLAYER_RADIUS = 0.4f;
constexpr float PLAYER_SPEED = 7.0f;
constexpr float PLAYER_SPRINT_MULTIPLIER = 1.5f;
constexpr float PLAYER_JUMP_FORCE = 8.0f;
constexpr float GRAVITY = 20.0f;
constexpr int PLAYER_MAX_HEALTH = 100;
constexpr float EYE_HEIGHT = 1.6f;
constexpr float RESPAWN_TIME = 3.0f;

struct PlayerInput {
    Vector3 moveDirection;  // Normalized movement direction
    Vector2 lookDelta;      // Mouse delta for looking
    bool jump;
    bool sprint;
    bool primaryFire;
    bool secondaryFire;
    bool reload;
    int weaponSlot;         // 0 = no change, 1-2 = weapon slot
    uint32_t sequence;      // Input sequence number for reconciliation
};

struct PlayerState {
    Vector3 position;
    Vector3 velocity;
    float yaw;              // Horizontal rotation (degrees)
    float pitch;            // Vertical rotation (degrees)
    int health;
    int currentWeapon;
    bool isGrounded;
    bool isDead;
    float respawnTimer;
    uint32_t lastProcessedInput;
};

class Player {
public:
    Player(uint8_t id, const std::string& name);
    ~Player();

    // Core updates
    void update(float deltaTime);
    void processInput(const PlayerInput& input, float deltaTime);

    // State
    uint8_t getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    const PlayerState& getState() const { return m_state; }
    PlayerState& getState() { return m_state; }

    // Position and rotation
    Vector3 getPosition() const { return m_state.position; }
    void setPosition(const Vector3& pos) { m_state.position = pos; }
    Vector3 getEyePosition() const;
    Vector3 getForward() const;
    Vector3 getRight() const;

    // Health
    int getHealth() const { return m_state.health; }
    void setHealth(int health);
    void takeDamage(int damage, uint8_t attackerId);
    void heal(int amount);
    bool isDead() const { return m_state.isDead; }
    void respawn(const Vector3& spawnPos);

    // Weapons
    Weapon* getCurrentWeapon();
    void switchWeapon(int slot);
    bool canShoot() const;
    void shoot();

    // Camera
    Camera3D getCamera() const;

    // Network interpolation
    void setTargetState(const PlayerState& state);
    void interpolate(float alpha);

private:
    uint8_t m_id;
    std::string m_name;
    PlayerState m_state;
    PlayerState m_previousState;  // For interpolation
    PlayerState m_targetState;    // For interpolation

    std::array<std::unique_ptr<Weapon>, 2> m_weapons;

    // Physics
    void applyGravity(float deltaTime);
    void applyMovement(const Vector3& direction, float deltaTime, bool sprint);
    void checkGrounded();
};

} // namespace arena
