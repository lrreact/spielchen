#include "Weapon.h"
#include <algorithm>

namespace arena {

Weapon::Weapon(WeaponType type)
    : m_type(type)
    , m_fireCooldown(0.0f)
    , m_reloadTimer(0.0f)
    , m_isReloading(false)
{
    switch (type) {
        case WeaponType::RIFLE:
            m_stats = RIFLE_STATS;
            m_name = "Rifle";
            m_currentAmmo = m_stats.magazineSize;
            m_reserveAmmo = m_stats.magazineSize * 3; // 3 extra mags
            break;

        case WeaponType::KNIFE:
            m_stats = KNIFE_STATS;
            m_name = "Knife";
            m_currentAmmo = -1; // Unlimited
            m_reserveAmmo = -1;
            break;
    }
}

Weapon::~Weapon() = default;

void Weapon::update(float deltaTime)
{
    // Update fire cooldown
    if (m_fireCooldown > 0.0f) {
        m_fireCooldown -= deltaTime;
        if (m_fireCooldown < 0.0f) {
            m_fireCooldown = 0.0f;
        }
    }

    // Update reload timer
    if (m_isReloading) {
        m_reloadTimer -= deltaTime;
        if (m_reloadTimer <= 0.0f) {
            // Reload complete
            m_isReloading = false;
            m_reloadTimer = 0.0f;

            if (m_stats.magazineSize > 0) {
                int ammoNeeded = m_stats.magazineSize - m_currentAmmo;
                int ammoToLoad = std::min(ammoNeeded, m_reserveAmmo);
                m_currentAmmo += ammoToLoad;
                m_reserveAmmo -= ammoToLoad;
            }
        }
    }
}

bool Weapon::canFire() const
{
    if (m_isReloading) return false;
    if (m_fireCooldown > 0.0f) return false;

    // Check ammo (negative means unlimited)
    if (m_stats.magazineSize > 0 && m_currentAmmo <= 0) {
        return false;
    }

    return true;
}

bool Weapon::fire()
{
    if (!canFire()) {
        return false;
    }

    // Consume ammo
    if (m_stats.magazineSize > 0) {
        m_currentAmmo--;
    }

    // Set cooldown
    m_fireCooldown = getFireInterval();

    return true;
}

void Weapon::reload()
{
    // Can't reload if already reloading, has full mag, or has no reserve ammo
    if (m_isReloading) return;
    if (m_stats.magazineSize <= 0) return; // Melee weapon
    if (m_currentAmmo >= m_stats.magazineSize) return;
    if (m_reserveAmmo <= 0) return;

    m_isReloading = true;
    m_reloadTimer = m_stats.reloadTime;
}

void Weapon::cancelReload()
{
    m_isReloading = false;
    m_reloadTimer = 0.0f;
}

float Weapon::getReloadProgress() const
{
    if (!m_isReloading || m_stats.reloadTime <= 0.0f) {
        return 1.0f;
    }
    return 1.0f - (m_reloadTimer / m_stats.reloadTime);
}

void Weapon::addReserveAmmo(int amount)
{
    if (m_stats.magazineSize > 0) {
        m_reserveAmmo += amount;
    }
}

float Weapon::getFireInterval() const
{
    // Convert RPM to seconds per shot
    return 60.0f / m_stats.fireRate;
}

} // namespace arena
