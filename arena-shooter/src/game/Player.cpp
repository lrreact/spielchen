#include "Player.h"
#include <algorithm>
#include <cmath>

namespace arena {

Player::Player(uint8_t id, const std::string& name)
    : m_id(id)
    , m_name(name)
{
    // Initialize state
    m_state.position = { 0.0f, 0.0f, 0.0f };
    m_state.velocity = { 0.0f, 0.0f, 0.0f };
    m_state.yaw = 0.0f;
    m_state.pitch = 0.0f;
    m_state.health = PLAYER_MAX_HEALTH;
    m_state.currentWeapon = 0;
    m_state.isGrounded = false;
    m_state.isDead = false;
    m_state.respawnTimer = 0.0f;
    m_state.lastProcessedInput = 0;

    m_previousState = m_state;
    m_targetState = m_state;

    // Initialize weapons
    m_weapons[0] = std::make_unique<Weapon>(WeaponType::RIFLE);
    m_weapons[1] = std::make_unique<Weapon>(WeaponType::KNIFE);
}

Player::~Player() = default;

void Player::update(float deltaTime)
{
    if (m_state.isDead) {
        m_state.respawnTimer -= deltaTime;
        return;
    }

    // Update weapons
    for (auto& weapon : m_weapons) {
        if (weapon) {
            weapon->update(deltaTime);
        }
    }
}

void Player::processInput(const PlayerInput& input, float deltaTime)
{
    if (m_state.isDead) {
        return;
    }

    // Apply look rotation
    m_state.yaw -= input.lookDelta.x;
    m_state.pitch -= input.lookDelta.y;

    // Clamp pitch to prevent over-rotation
    m_state.pitch = std::clamp(m_state.pitch, -89.0f, 89.0f);

    // Normalize yaw
    while (m_state.yaw > 360.0f) m_state.yaw -= 360.0f;
    while (m_state.yaw < 0.0f) m_state.yaw += 360.0f;

    // Calculate movement direction in world space
    if (Vector3Length(input.moveDirection) > 0.0f) {
        Vector3 moveDir = Vector3Normalize(input.moveDirection);

        // Transform movement direction by yaw
        float yawRad = m_state.yaw * DEG2RAD;
        Vector3 worldMove = {
            moveDir.x * cosf(yawRad) - moveDir.z * sinf(yawRad),
            0.0f,
            moveDir.x * sinf(yawRad) + moveDir.z * cosf(yawRad)
        };

        applyMovement(worldMove, deltaTime, input.sprint);
    }

    // Apply gravity
    applyGravity(deltaTime);

    // Jump
    if (input.jump && m_state.isGrounded) {
        m_state.velocity.y = PLAYER_JUMP_FORCE;
        m_state.isGrounded = false;
    }

    // Apply velocity to position
    m_state.position.x += m_state.velocity.x * deltaTime;
    m_state.position.y += m_state.velocity.y * deltaTime;
    m_state.position.z += m_state.velocity.z * deltaTime;

    // Ground check (simple: y = 0 is ground)
    checkGrounded();

    // Weapon switch
    if (input.weaponSlot > 0 && input.weaponSlot <= 2) {
        switchWeapon(input.weaponSlot - 1);
    }

    // Reload
    if (input.reload) {
        Weapon* weapon = getCurrentWeapon();
        if (weapon) {
            weapon->reload();
        }
    }

    // Shooting
    if (input.primaryFire) {
        shoot();
    }

    m_state.lastProcessedInput = input.sequence;
}

Vector3 Player::getEyePosition() const
{
    return {
        m_state.position.x,
        m_state.position.y + EYE_HEIGHT,
        m_state.position.z
    };
}

Vector3 Player::getForward() const
{
    float yawRad = m_state.yaw * DEG2RAD;
    float pitchRad = m_state.pitch * DEG2RAD;

    return {
        cosf(pitchRad) * sinf(yawRad),
        -sinf(pitchRad),
        cosf(pitchRad) * cosf(yawRad)
    };
}

Vector3 Player::getRight() const
{
    float yawRad = m_state.yaw * DEG2RAD;
    return {
        cosf(yawRad),
        0.0f,
        -sinf(yawRad)
    };
}

void Player::setHealth(int health)
{
    m_state.health = std::clamp(health, 0, PLAYER_MAX_HEALTH);
    if (m_state.health <= 0) {
        m_state.isDead = true;
        m_state.respawnTimer = RESPAWN_TIME;
    }
}

void Player::takeDamage(int damage, uint8_t attackerId)
{
    if (m_state.isDead) return;

    setHealth(m_state.health - damage);
}

void Player::heal(int amount)
{
    if (m_state.isDead) return;

    setHealth(m_state.health + amount);
}

void Player::respawn(const Vector3& spawnPos)
{
    m_state.position = spawnPos;
    m_state.velocity = { 0.0f, 0.0f, 0.0f };
    m_state.health = PLAYER_MAX_HEALTH;
    m_state.isDead = false;
    m_state.respawnTimer = 0.0f;
    m_state.currentWeapon = 0;

    // Reset weapons
    for (auto& weapon : m_weapons) {
        if (weapon) {
            weapon->reload();
        }
    }
}

Weapon* Player::getCurrentWeapon()
{
    if (m_state.currentWeapon >= 0 &&
        m_state.currentWeapon < static_cast<int>(m_weapons.size())) {
        return m_weapons[m_state.currentWeapon].get();
    }
    return nullptr;
}

void Player::switchWeapon(int slot)
{
    if (slot >= 0 && slot < static_cast<int>(m_weapons.size())) {
        // Cancel reload when switching
        Weapon* current = getCurrentWeapon();
        if (current) {
            current->cancelReload();
        }
        m_state.currentWeapon = slot;
    }
}

bool Player::canShoot() const
{
    if (m_state.isDead) return false;

    // Cast away const for getCurrentWeapon (it doesn't modify state)
    const Weapon* weapon = const_cast<Player*>(this)->getCurrentWeapon();
    return weapon && weapon->canFire();
}

void Player::shoot()
{
    Weapon* weapon = getCurrentWeapon();
    if (weapon && weapon->canFire()) {
        weapon->fire();
        // Hit detection is handled by server/game
    }
}

Camera3D Player::getCamera() const
{
    Camera3D camera = { 0 };
    camera.position = getEyePosition();
    camera.target = Vector3Add(camera.position, getForward());
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    return camera;
}

void Player::setTargetState(const PlayerState& state)
{
    m_previousState = m_state;
    m_targetState = state;
}

void Player::interpolate(float alpha)
{
    // Interpolate position
    m_state.position.x = m_previousState.position.x +
        alpha * (m_targetState.position.x - m_previousState.position.x);
    m_state.position.y = m_previousState.position.y +
        alpha * (m_targetState.position.y - m_previousState.position.y);
    m_state.position.z = m_previousState.position.z +
        alpha * (m_targetState.position.z - m_previousState.position.z);

    // Interpolate rotation (simple lerp, could use slerp for large differences)
    m_state.yaw = m_previousState.yaw +
        alpha * (m_targetState.yaw - m_previousState.yaw);
    m_state.pitch = m_previousState.pitch +
        alpha * (m_targetState.pitch - m_previousState.pitch);
}

void Player::applyGravity(float deltaTime)
{
    if (!m_state.isGrounded) {
        m_state.velocity.y -= GRAVITY * deltaTime;
    }
}

void Player::applyMovement(const Vector3& direction, float deltaTime, bool sprint)
{
    float speed = PLAYER_SPEED;
    if (sprint) {
        speed *= PLAYER_SPRINT_MULTIPLIER;
    }

    // Set horizontal velocity directly (no acceleration for responsive feel)
    m_state.velocity.x = direction.x * speed;
    m_state.velocity.z = direction.z * speed;
}

void Player::checkGrounded()
{
    // Simple ground check at y = 0
    if (m_state.position.y <= 0.0f) {
        m_state.position.y = 0.0f;
        m_state.velocity.y = 0.0f;
        m_state.isGrounded = true;
    } else {
        m_state.isGrounded = false;
    }
}

// Respawn time constant
constexpr float RESPAWN_TIME = 3.0f;

} // namespace arena
