#pragma once

#include "raylib.h"
#include <vector>
#include <string>

namespace arena {

// Map constants
constexpr float MAP_SIZE = 50.0f;
constexpr int NUM_SPAWN_POINTS = 8;

struct SpawnPoint {
    Vector3 position;
    float rotation;         // Yaw in degrees
};

struct MapCollider {
    BoundingBox bounds;
    bool isSolid;
};

class Map {
public:
    Map();
    ~Map();

    // Loading
    bool loadFromFile(const std::string& filepath);
    void generateDefaultArena();

    // Rendering
    void render() const;
    void renderDebug() const;

    // Collision
    bool checkCollision(const Vector3& point, float radius) const;
    bool checkLineOfSight(const Vector3& from, const Vector3& to) const;
    Vector3 resolveCollision(const Vector3& position, const Vector3& velocity,
                             float radius) const;

    // Raycast for hitscan weapons
    struct RaycastHit {
        bool hit;
        Vector3 point;
        Vector3 normal;
        float distance;
    };
    RaycastHit raycast(const Ray& ray, float maxDistance) const;

    // Spawn points
    const SpawnPoint& getRandomSpawnPoint() const;
    const std::vector<SpawnPoint>& getSpawnPoints() const { return m_spawnPoints; }

    // Bounds
    BoundingBox getBounds() const { return m_bounds; }

private:
    Model m_model;
    bool m_modelLoaded;

    std::vector<MapCollider> m_colliders;
    std::vector<SpawnPoint> m_spawnPoints;
    BoundingBox m_bounds;

    // Procedural geometry for default arena
    std::vector<BoundingBox> m_walls;
    std::vector<BoundingBox> m_platforms;
    std::vector<BoundingBox> m_covers;

    void generateColliders();
    void generateSpawnPoints();
};

} // namespace arena
