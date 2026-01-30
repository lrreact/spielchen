#pragma once

#include "raylib.h"
#include <cstdint>

namespace arena {

// Currently unused since weapons are hitscan, but prepared for future expansion
struct Projectile {
    Vector3 position;
    Vector3 velocity;
    float lifetime;
    int damage;
    uint8_t ownerId;
    bool active;
};

} // namespace arena
