#include "Map.h"
#include "rlgl.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

namespace arena {

Map::Map()
    : m_modelLoaded(false)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

Map::~Map()
{
    if (m_modelLoaded) {
        UnloadModel(m_model);
    }
}

bool Map::loadFromFile(const std::string& filepath)
{
    if (m_modelLoaded) {
        UnloadModel(m_model);
    }

    m_model = LoadModel(filepath.c_str());
    m_modelLoaded = true;

    generateColliders();
    generateSpawnPoints();

    return true;
}

void Map::generateDefaultArena()
{
    // Clear existing geometry
    m_walls.clear();
    m_platforms.clear();
    m_covers.clear();
    m_colliders.clear();
    m_spawnPoints.clear();

    // Arena bounds
    float halfSize = MAP_SIZE / 2.0f;
    m_bounds = {
        { -halfSize, 0.0f, -halfSize },
        { halfSize, 20.0f, halfSize }
    };

    // Outer walls (4 walls surrounding the arena)
    float wallThickness = 1.0f;
    float wallHeight = 5.0f;

    // North wall
    m_walls.push_back({
        { -halfSize, 0.0f, halfSize - wallThickness },
        { halfSize, wallHeight, halfSize }
    });
    // South wall
    m_walls.push_back({
        { -halfSize, 0.0f, -halfSize },
        { halfSize, wallHeight, -halfSize + wallThickness }
    });
    // East wall
    m_walls.push_back({
        { halfSize - wallThickness, 0.0f, -halfSize },
        { halfSize, wallHeight, halfSize }
    });
    // West wall
    m_walls.push_back({
        { -halfSize, 0.0f, -halfSize },
        { -halfSize + wallThickness, wallHeight, halfSize }
    });

    // Central structure with upper platform
    float centerSize = 8.0f;
    float platformHeight = 3.0f;

    // Central pillars (4 corners)
    float pillarSize = 1.5f;
    float pillarOffset = centerSize / 2.0f - pillarSize / 2.0f;

    m_walls.push_back({
        { -pillarOffset - pillarSize/2, 0.0f, -pillarOffset - pillarSize/2 },
        { -pillarOffset + pillarSize/2, platformHeight + 1.0f, -pillarOffset + pillarSize/2 }
    });
    m_walls.push_back({
        { pillarOffset - pillarSize/2, 0.0f, -pillarOffset - pillarSize/2 },
        { pillarOffset + pillarSize/2, platformHeight + 1.0f, -pillarOffset + pillarSize/2 }
    });
    m_walls.push_back({
        { -pillarOffset - pillarSize/2, 0.0f, pillarOffset - pillarSize/2 },
        { -pillarOffset + pillarSize/2, platformHeight + 1.0f, pillarOffset + pillarSize/2 }
    });
    m_walls.push_back({
        { pillarOffset - pillarSize/2, 0.0f, pillarOffset - pillarSize/2 },
        { pillarOffset + pillarSize/2, platformHeight + 1.0f, pillarOffset + pillarSize/2 }
    });

    // Central platform
    m_platforms.push_back({
        { -centerSize/2, platformHeight, -centerSize/2 },
        { centerSize/2, platformHeight + 0.5f, centerSize/2 }
    });

    // Scattered cover objects (crates)
    float crateSize = 1.5f;
    float cratePositions[][2] = {
        { -15.0f, -15.0f },
        { -15.0f, 15.0f },
        { 15.0f, -15.0f },
        { 15.0f, 15.0f },
        { -10.0f, 0.0f },
        { 10.0f, 0.0f },
        { 0.0f, -10.0f },
        { 0.0f, 10.0f },
        { -20.0f, -5.0f },
        { 20.0f, 5.0f },
        { -5.0f, -20.0f },
        { 5.0f, 20.0f },
    };

    for (const auto& pos : cratePositions) {
        m_covers.push_back({
            { pos[0] - crateSize/2, 0.0f, pos[1] - crateSize/2 },
            { pos[0] + crateSize/2, crateSize, pos[1] + crateSize/2 }
        });
    }

    // Generate colliders from geometry
    for (const auto& wall : m_walls) {
        m_colliders.push_back({ wall, true });
    }
    for (const auto& platform : m_platforms) {
        m_colliders.push_back({ platform, true });
    }
    for (const auto& cover : m_covers) {
        m_colliders.push_back({ cover, true });
    }

    // Generate spawn points
    generateSpawnPoints();
}

void Map::render() const
{
    // Draw ground
    float halfSize = MAP_SIZE / 2.0f;
    DrawPlane({ 0.0f, 0.0f, 0.0f }, { MAP_SIZE, MAP_SIZE }, DARKGRAY);

    // Draw grid on ground
    DrawGrid(static_cast<int>(MAP_SIZE), 1.0f);

    // Draw walls
    for (const auto& wall : m_walls) {
        Vector3 center = {
            (wall.min.x + wall.max.x) / 2.0f,
            (wall.min.y + wall.max.y) / 2.0f,
            (wall.min.z + wall.max.z) / 2.0f
        };
        Vector3 size = {
            wall.max.x - wall.min.x,
            wall.max.y - wall.min.y,
            wall.max.z - wall.min.z
        };
        DrawCubeV(center, size, GRAY);
        DrawCubeWiresV(center, size, DARKGRAY);
    }

    // Draw platforms
    for (const auto& platform : m_platforms) {
        Vector3 center = {
            (platform.min.x + platform.max.x) / 2.0f,
            (platform.min.y + platform.max.y) / 2.0f,
            (platform.min.z + platform.max.z) / 2.0f
        };
        Vector3 size = {
            platform.max.x - platform.min.x,
            platform.max.y - platform.min.y,
            platform.max.z - platform.min.z
        };
        DrawCubeV(center, size, BROWN);
        DrawCubeWiresV(center, size, DARKBROWN);
    }

    // Draw cover objects
    for (const auto& cover : m_covers) {
        Vector3 center = {
            (cover.min.x + cover.max.x) / 2.0f,
            (cover.min.y + cover.max.y) / 2.0f,
            (cover.min.z + cover.max.z) / 2.0f
        };
        Vector3 size = {
            cover.max.x - cover.min.x,
            cover.max.y - cover.min.y,
            cover.max.z - cover.min.z
        };
        DrawCubeV(center, size, BEIGE);
        DrawCubeWiresV(center, size, BROWN);
    }
}

void Map::renderDebug() const
{
    // Draw spawn points
    for (const auto& spawn : m_spawnPoints) {
        DrawSphere(spawn.position, 0.3f, GREEN);

        // Draw direction indicator
        float yawRad = spawn.rotation * DEG2RAD;
        Vector3 dir = {
            sinf(yawRad),
            0.0f,
            cosf(yawRad)
        };
        Vector3 end = Vector3Add(spawn.position, Vector3Scale(dir, 1.0f));
        DrawLine3D(spawn.position, end, YELLOW);
    }

    // Draw collider bounds
    for (const auto& collider : m_colliders) {
        DrawBoundingBox(collider.bounds, RED);
    }
}

bool Map::checkCollision(const Vector3& point, float radius) const
{
    // Create sphere bounding box
    BoundingBox sphereBounds = {
        { point.x - radius, point.y - radius, point.z - radius },
        { point.x + radius, point.y + radius, point.z + radius }
    };

    for (const auto& collider : m_colliders) {
        if (collider.isSolid && CheckCollisionBoxes(sphereBounds, collider.bounds)) {
            return true;
        }
    }

    return false;
}

bool Map::checkLineOfSight(const Vector3& from, const Vector3& to) const
{
    Ray ray = { from, Vector3Normalize(Vector3Subtract(to, from)) };
    float distance = Vector3Distance(from, to);

    RaycastHit hit = raycast(ray, distance);
    return !hit.hit;
}

Vector3 Map::resolveCollision(const Vector3& position, const Vector3& velocity,
                              float radius) const
{
    Vector3 newPos = Vector3Add(position, velocity);

    // Simple collision resolution: push out of colliders
    for (const auto& collider : m_colliders) {
        if (!collider.isSolid) continue;

        BoundingBox expanded = {
            { collider.bounds.min.x - radius, collider.bounds.min.y - radius,
              collider.bounds.min.z - radius },
            { collider.bounds.max.x + radius, collider.bounds.max.y + radius,
              collider.bounds.max.z + radius }
        };

        if (newPos.x >= expanded.min.x && newPos.x <= expanded.max.x &&
            newPos.y >= expanded.min.y && newPos.y <= expanded.max.y &&
            newPos.z >= expanded.min.z && newPos.z <= expanded.max.z) {

            // Find closest face and push out
            float distances[6] = {
                newPos.x - expanded.min.x,  // Left
                expanded.max.x - newPos.x,  // Right
                newPos.y - expanded.min.y,  // Bottom
                expanded.max.y - newPos.y,  // Top
                newPos.z - expanded.min.z,  // Back
                expanded.max.z - newPos.z   // Front
            };

            int minIdx = 0;
            for (int i = 1; i < 6; i++) {
                if (distances[i] < distances[minIdx]) {
                    minIdx = i;
                }
            }

            switch (minIdx) {
                case 0: newPos.x = expanded.min.x; break;
                case 1: newPos.x = expanded.max.x; break;
                case 2: newPos.y = expanded.min.y; break;
                case 3: newPos.y = expanded.max.y; break;
                case 4: newPos.z = expanded.min.z; break;
                case 5: newPos.z = expanded.max.z; break;
            }
        }
    }

    return newPos;
}

Map::RaycastHit Map::raycast(const Ray& ray, float maxDistance) const
{
    RaycastHit result = { false, { 0 }, { 0 }, maxDistance };

    for (const auto& collider : m_colliders) {
        if (!collider.isSolid) continue;

        RayCollision collision = GetRayCollisionBox(ray, collider.bounds);

        if (collision.hit && collision.distance < result.distance &&
            collision.distance <= maxDistance) {
            result.hit = true;
            result.point = collision.point;
            result.normal = collision.normal;
            result.distance = collision.distance;
        }
    }

    return result;
}

const SpawnPoint& Map::getRandomSpawnPoint() const
{
    int index = std::rand() % m_spawnPoints.size();
    return m_spawnPoints[index];
}

void Map::generateColliders()
{
    // Colliders are generated in generateDefaultArena or from model
}

void Map::generateSpawnPoints()
{
    m_spawnPoints.clear();

    // 8 spawn points distributed around the arena
    float spawnRadius = MAP_SIZE / 2.0f - 5.0f; // 5m from walls

    for (int i = 0; i < NUM_SPAWN_POINTS; i++) {
        float angle = (360.0f / NUM_SPAWN_POINTS) * i;
        float rad = angle * DEG2RAD;

        SpawnPoint spawn;
        spawn.position = {
            cosf(rad) * spawnRadius,
            0.0f,
            sinf(rad) * spawnRadius
        };
        // Face towards center
        spawn.rotation = angle + 180.0f;
        if (spawn.rotation >= 360.0f) spawn.rotation -= 360.0f;

        m_spawnPoints.push_back(spawn);
    }
}

} // namespace arena
