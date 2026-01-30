#include "Renderer.h"
#include "rlgl.h"
#include <algorithm>

namespace arena {

Renderer::Renderer()
    : m_screenWidth(1280)
    , m_screenHeight(720)
    , m_fov(60.0f)
    , m_modelsLoaded(false)
    , m_showHitMarker(false)
    , m_hitMarkerTimer(0.0f)
{
}

Renderer::~Renderer()
{
    shutdown();
}

bool Renderer::init(int width, int height, const char* title)
{
    m_screenWidth = width;
    m_screenHeight = height;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(width, height, title);

    if (!IsWindowReady()) {
        return false;
    }

    SetTargetFPS(60);

    loadModels();

    return true;
}

void Renderer::shutdown()
{
    unloadModels();

    if (IsWindowReady()) {
        CloseWindow();
    }
}

void Renderer::beginFrame()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);
}

void Renderer::endFrame()
{
    EndDrawing();
}

void Renderer::renderWorld(const Map* map, const std::vector<Player*>& players,
                           const Camera3D& camera)
{
    BeginMode3D(camera);

    // Render map
    if (map) {
        map->render();
    }

    // Render players
    for (const Player* player : players) {
        if (player) {
            renderPlayer(player, false);
        }
    }

    EndMode3D();
}

void Renderer::renderPlayer(const Player* player, bool isLocal)
{
    if (!player || player->isDead()) {
        return;
    }

    Vector3 pos = player->getPosition();

    if (isLocal) {
        // Don't render own body in first person
        return;
    }

    // Simple capsule representation
    float radius = PLAYER_RADIUS;
    float height = PLAYER_HEIGHT;

    // Body (cylinder approximation with spheres)
    DrawCapsule(
        { pos.x, pos.y + radius, pos.z },
        { pos.x, pos.y + height - radius, pos.z },
        radius,
        8, 8,
        BLUE
    );

    // Head
    DrawSphere(
        { pos.x, pos.y + height - radius * 0.5f, pos.z },
        radius * 0.7f,
        SKYBLUE
    );

    // Direction indicator
    Vector3 forward = player->getForward();
    forward.y = 0;
    forward = Vector3Normalize(forward);

    Vector3 indicatorStart = { pos.x, pos.y + height * 0.7f, pos.z };
    Vector3 indicatorEnd = Vector3Add(indicatorStart, Vector3Scale(forward, 0.5f));
    DrawLine3D(indicatorStart, indicatorEnd, YELLOW);
}

void Renderer::renderWeaponViewModel(const Weapon* weapon, const Camera3D& camera)
{
    if (!weapon) {
        return;
    }

    // Weapon viewmodel position (bottom right of screen)
    // This is a simplified 2D representation; proper 3D viewmodels would need
    // separate rendering pass with different projection

    int weaponX = m_screenWidth - 150;
    int weaponY = m_screenHeight - 100;

    switch (weapon->getType()) {
        case WeaponType::RIFLE:
            // Draw simplified rifle shape
            DrawRectangle(weaponX, weaponY, 100, 20, DARKGRAY);
            DrawRectangle(weaponX + 80, weaponY - 15, 15, 35, DARKGRAY);
            DrawRectangle(weaponX + 10, weaponY + 15, 30, 20, BROWN);
            break;

        case WeaponType::KNIFE:
            // Draw simplified knife shape
            DrawTriangle(
                { (float)weaponX + 50, (float)weaponY },
                { (float)weaponX + 30, (float)weaponY + 40 },
                { (float)weaponX + 70, (float)weaponY + 40 },
                LIGHTGRAY
            );
            DrawRectangle(weaponX + 40, weaponY + 40, 20, 30, BROWN);
            break;
    }
}

void Renderer::renderHUD(const Player* localPlayer)
{
    if (!localPlayer) {
        return;
    }

    renderCrosshair();
    renderHealthBar(localPlayer->getHealth(), PLAYER_MAX_HEALTH);

    Weapon* weapon = const_cast<Player*>(localPlayer)->getCurrentWeapon();
    if (weapon) {
        renderAmmoCounter(weapon);
        renderWeaponViewModel(weapon, localPlayer->getCamera());
    }

    renderKillfeed();

    // Hit marker
    if (m_showHitMarker) {
        renderHitMarker();
    }
}

void Renderer::renderCrosshair()
{
    int centerX = m_screenWidth / 2;
    int centerY = m_screenHeight / 2;
    int size = 10;
    int gap = 4;
    int thickness = 2;

    Color crosshairColor = WHITE;

    // Horizontal lines
    DrawRectangle(centerX - size - gap, centerY - thickness/2, size, thickness, crosshairColor);
    DrawRectangle(centerX + gap, centerY - thickness/2, size, thickness, crosshairColor);

    // Vertical lines
    DrawRectangle(centerX - thickness/2, centerY - size - gap, thickness, size, crosshairColor);
    DrawRectangle(centerX - thickness/2, centerY + gap, thickness, size, crosshairColor);

    // Center dot (optional)
    DrawCircle(centerX, centerY, 2, crosshairColor);
}

void Renderer::renderHealthBar(int health, int maxHealth)
{
    int barWidth = 200;
    int barHeight = 20;
    int x = HUD_PADDING;
    int y = m_screenHeight - HUD_PADDING - barHeight;

    // Background
    DrawRectangle(x - 2, y - 2, barWidth + 4, barHeight + 4, BLACK);

    // Health bar
    float healthPercent = static_cast<float>(health) / maxHealth;
    int healthWidth = static_cast<int>(barWidth * healthPercent);

    Color healthColor = GREEN;
    if (healthPercent < 0.3f) healthColor = RED;
    else if (healthPercent < 0.6f) healthColor = ORANGE;

    DrawRectangle(x, y, healthWidth, barHeight, healthColor);
    DrawRectangleLines(x, y, barWidth, barHeight, WHITE);

    // Health text
    DrawText(TextFormat("%d", health), x + barWidth + 10, y, 20, WHITE);
}

void Renderer::renderAmmoCounter(const Weapon* weapon)
{
    if (!weapon) return;

    int x = m_screenWidth - HUD_PADDING - 150;
    int y = m_screenHeight - HUD_PADDING - 40;

    // Weapon name
    DrawText(weapon->getName().c_str(), x, y - 25, 16, LIGHTGRAY);

    if (weapon->getStats().magazineSize > 0) {
        // Ammo count
        const char* ammoText = TextFormat("%d / %d",
            weapon->getAmmo(), weapon->getReserveAmmo());
        DrawText(ammoText, x, y, 24, WHITE);

        // Reload indicator
        if (weapon->isReloading()) {
            float progress = weapon->getReloadProgress();
            DrawRectangle(x, y + 30, 100, 5, DARKGRAY);
            DrawRectangle(x, y + 30, static_cast<int>(100 * progress), 5, YELLOW);
            DrawText("RELOADING", x, y + 40, 12, YELLOW);
        }
    } else {
        DrawText("MELEE", x, y, 24, WHITE);
    }
}

void Renderer::renderKillfeed()
{
    int x = m_screenWidth - HUD_PADDING - 300;
    int y = HUD_PADDING;
    int lineHeight = 20;

    for (size_t i = 0; i < m_killfeed.size() && i < KILLFEED_MAX_ENTRIES; i++) {
        const KillfeedEntry& entry = m_killfeed[i];

        // Fade based on time
        float alpha = std::min(1.0f, entry.timer / 2.0f);
        Color textColor = Fade(WHITE, alpha);

        const char* weaponIcon = (entry.weapon == WeaponType::KNIFE) ? "[KNIFE]" : "[RIFLE]";

        DrawText(TextFormat("%s %s %s",
            entry.killerName.c_str(),
            weaponIcon,
            entry.victimName.c_str()),
            x, y + static_cast<int>(i * lineHeight), 14, textColor);
    }
}

void Renderer::renderScoreboard(const std::vector<Player*>& players)
{
    // Semi-transparent background
    DrawRectangle(m_screenWidth/4, m_screenHeight/4,
                  m_screenWidth/2, m_screenHeight/2,
                  Fade(BLACK, 0.8f));

    // Header
    DrawText("SCOREBOARD", m_screenWidth/2 - 60, m_screenHeight/4 + 20, 24, WHITE);
    DrawText("NAME", m_screenWidth/4 + 30, m_screenHeight/4 + 60, 16, LIGHTGRAY);
    DrawText("KILLS", m_screenWidth/2 - 20, m_screenHeight/4 + 60, 16, LIGHTGRAY);
    DrawText("DEATHS", m_screenWidth/2 + 60, m_screenHeight/4 + 60, 16, LIGHTGRAY);
    DrawText("PING", m_screenWidth*3/4 - 60, m_screenHeight/4 + 60, 16, LIGHTGRAY);

    // Player list
    int y = m_screenHeight/4 + 90;
    for (const Player* player : players) {
        if (!player) continue;

        DrawText(player->getName().c_str(),
                 m_screenWidth/4 + 30, y, 14, WHITE);
        // TODO: Add kill/death tracking
        DrawText("0", m_screenWidth/2 - 10, y, 14, WHITE);
        DrawText("0", m_screenWidth/2 + 70, y, 14, WHITE);
        DrawText("--", m_screenWidth*3/4 - 50, y, 14, WHITE);

        y += 25;
    }
}

void Renderer::renderMuzzleFlash(const Vector3& position, const Vector3& direction)
{
    // Simple flash effect
    DrawSphere(position, 0.1f, YELLOW);
}

void Renderer::renderHitMarker()
{
    int centerX = m_screenWidth / 2;
    int centerY = m_screenHeight / 2;
    int size = 15;

    // X shape
    DrawLine(centerX - size, centerY - size, centerX - 5, centerY - 5, WHITE);
    DrawLine(centerX + size, centerY - size, centerX + 5, centerY - 5, WHITE);
    DrawLine(centerX - size, centerY + size, centerX - 5, centerY + 5, WHITE);
    DrawLine(centerX + size, centerY + size, centerX + 5, centerY + 5, WHITE);
}

void Renderer::renderDamageIndicator(const Vector3& damageSource)
{
    // TODO: Implement directional damage indicator
}

void Renderer::addKillfeedEntry(const std::string& killer, const std::string& victim,
                                WeaponType weapon)
{
    KillfeedEntry entry;
    entry.killerName = killer;
    entry.victimName = victim;
    entry.weapon = weapon;
    entry.timer = KILLFEED_DURATION;

    m_killfeed.insert(m_killfeed.begin(), entry);

    // Limit size
    while (m_killfeed.size() > KILLFEED_MAX_ENTRIES) {
        m_killfeed.pop_back();
    }
}

void Renderer::updateKillfeed(float deltaTime)
{
    for (auto& entry : m_killfeed) {
        entry.timer -= deltaTime;
    }

    // Remove expired entries
    m_killfeed.erase(
        std::remove_if(m_killfeed.begin(), m_killfeed.end(),
            [](const KillfeedEntry& e) { return e.timer <= 0.0f; }),
        m_killfeed.end()
    );

    // Update hit marker
    if (m_showHitMarker) {
        m_hitMarkerTimer -= deltaTime;
        if (m_hitMarkerTimer <= 0.0f) {
            m_showHitMarker = false;
        }
    }
}

void Renderer::loadModels()
{
    // TODO: Load actual models from assets
    // For now, we use procedural rendering
    m_modelsLoaded = false;
}

void Renderer::unloadModels()
{
    if (m_modelsLoaded) {
        UnloadModel(m_playerModel);
        UnloadModel(m_rifleModel);
        UnloadModel(m_knifeModel);
        m_modelsLoaded = false;
    }
}

Color Renderer::getTeamColor(int teamId)
{
    switch (teamId) {
        case 0: return RED;
        case 1: return BLUE;
        default: return WHITE;
    }
}

} // namespace arena
