#pragma once

#include "raylib.h"
#include <string>

namespace arena {

enum class WeaponType {
    RIFLE,
    KNIFE
};

struct WeaponStats {
    int damage;
    float fireRate;         // Rounds per minute (for automatic) or attacks per second
    int magazineSize;
    float reloadTime;       // Seconds
    float range;            // Max effective range in meters
    bool isAutomatic;
    bool isHitscan;         // true = instant hit, false = projectile
    bool isMelee;
};

// Weapon definitions
constexpr WeaponStats RIFLE_STATS = {
    .damage = 25,
    .fireRate = 600.0f,     // 600 RPM = 10 shots/sec
    .magazineSize = 30,
    .reloadTime = 2.0f,
    .range = 100.0f,
    .isAutomatic = true,
    .isHitscan = true,
    .isMelee = false
};

constexpr WeaponStats KNIFE_STATS = {
    .damage = 50,
    .fireRate = 60.0f,      // 1 attack per second
    .magazineSize = -1,     // Unlimited
    .reloadTime = 0.0f,
    .range = 2.0f,
    .isAutomatic = false,
    .isHitscan = true,
    .isMelee = true
};

class Weapon {
public:
    Weapon(WeaponType type);
    ~Weapon();

    // Core functions
    void update(float deltaTime);
    bool canFire() const;
    bool fire();            // Returns true if fired successfully
    void reload();
    void cancelReload();

    // Getters
    WeaponType getType() const { return m_type; }
    const WeaponStats& getStats() const { return m_stats; }
    const std::string& getName() const { return m_name; }

    int getAmmo() const { return m_currentAmmo; }
    int getReserveAmmo() const { return m_reserveAmmo; }
    bool isReloading() const { return m_isReloading; }
    float getReloadProgress() const;

    // State
    void setAmmo(int ammo) { m_currentAmmo = ammo; }
    void addReserveAmmo(int amount);

private:
    WeaponType m_type;
    WeaponStats m_stats;
    std::string m_name;

    int m_currentAmmo;
    int m_reserveAmmo;
    float m_fireCooldown;   // Time until next shot allowed
    float m_reloadTimer;
    bool m_isReloading;

    float getFireInterval() const;
};

} // namespace arena
