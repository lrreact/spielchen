#pragma once

#include "raylib.h"
#include <vector>

#include "game/Player.h"
#include "game/Map.h"
#include "game/Weapon.h"

namespace arena {

// HUD constants
constexpr int HUD_PADDING = 20;
constexpr int KILLFEED_MAX_ENTRIES = 5;
constexpr float KILLFEED_DURATION = 5.0f;

struct KillfeedEntry {
    std::string killerName;
    std::string victimName;
    WeaponType weapon;
    float timer;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initialization
    bool init(int width, int height, const char* title);
    void shutdown();

    // Main render
    void beginFrame();
    void endFrame();

    // 3D rendering
    void renderWorld(const Map* map, const std::vector<Player*>& players,
                     const Camera3D& camera);
    void renderPlayer(const Player* player, bool isLocal);
    void renderWeaponViewModel(const Weapon* weapon, const Camera3D& camera);

    // 2D/HUD rendering
    void renderHUD(const Player* localPlayer);
    void renderCrosshair();
    void renderHealthBar(int health, int maxHealth);
    void renderAmmoCounter(const Weapon* weapon);
    void renderKillfeed();
    void renderScoreboard(const std::vector<Player*>& players);

    // Effects
    void renderMuzzleFlash(const Vector3& position, const Vector3& direction);
    void renderHitMarker();
    void renderDamageIndicator(const Vector3& damageSource);

    // Killfeed
    void addKillfeedEntry(const std::string& killer, const std::string& victim,
                          WeaponType weapon);
    void updateKillfeed(float deltaTime);

    // Settings
    void setFOV(float fov) { m_fov = fov; }
    float getFOV() const { return m_fov; }

private:
    int m_screenWidth;
    int m_screenHeight;
    float m_fov;

    // Models
    Model m_playerModel;
    Model m_rifleModel;
    Model m_knifeModel;
    bool m_modelsLoaded;

    // Textures
    Texture2D m_crosshairTexture;
    Texture2D m_hitMarkerTexture;

    // Shaders (optional)
    Shader m_defaultShader;

    // Killfeed
    std::vector<KillfeedEntry> m_killfeed;

    // State
    bool m_showHitMarker;
    float m_hitMarkerTimer;

    // Helper functions
    void loadModels();
    void unloadModels();
    Color getTeamColor(int teamId);
};

} // namespace arena
