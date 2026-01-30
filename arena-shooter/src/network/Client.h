#pragma once

// IMPORTANT: Include raylib BEFORE enet on Windows to avoid API conflicts
#include "raylib.h"

#include <string>
#include <queue>
#include <enet/enet.h>

#include "Protocol.h"
#include "game/Player.h"

namespace arena {

enum class ConnectionState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    DISCONNECTING
};

class Client {
public:
    Client();
    ~Client();

    // Connection
    bool connect(const std::string& host, uint16_t port,
                 const std::string& playerName);
    void disconnect();
    void update(float deltaTime);

    // State
    ConnectionState getState() const { return m_state; }
    bool isConnected() const { return m_state == ConnectionState::CONNECTED; }
    uint8_t getLocalPlayerId() const { return m_localPlayerId; }
    float getPing() const { return m_ping; }

    // Input
    void sendInput(const PlayerInput& input);

    // Callbacks (set these to handle events)
    using WorldStateCallback = void(*)(const WorldStatePacket& state);
    using PlayerEventCallback = void(*)(uint8_t playerId);

    void setWorldStateCallback(WorldStateCallback cb) { m_onWorldState = cb; }
    void setPlayerJoinCallback(PlayerEventCallback cb) { m_onPlayerJoin = cb; }
    void setPlayerLeaveCallback(PlayerEventCallback cb) { m_onPlayerLeave = cb; }

private:
    ENetHost* m_client;
    ENetPeer* m_server;
    ConnectionState m_state;

    std::string m_playerName;
    uint8_t m_localPlayerId;
    float m_ping;

    uint16_t m_sequence;
    uint16_t m_lastAck;
    uint32_t m_ackBits;

    // Input history for reconciliation
    static constexpr int INPUT_BUFFER_SIZE = 64;
    std::queue<NetworkPlayerInput> m_inputHistory;
    uint32_t m_lastAckedInput;

    // Event handling
    void handleReceive(ENetEvent& event);
    void handleConnectAccept(const ConnectAcceptPacket& packet);
    void handleConnectReject(const ConnectRejectPacket& packet);
    void handleWorldState(const WorldStatePacket& packet);
    void handlePlayerHit(const PlayerHitPacket& packet);
    void handlePlayerDeath(const PlayerDeathPacket& packet);
    void handlePlayerSpawn(const PlayerSpawnPacket& packet);

    // Prediction & reconciliation
    void reconcileState(const NetworkPlayerState& serverState);

    // Callbacks
    WorldStateCallback m_onWorldState = nullptr;
    PlayerEventCallback m_onPlayerJoin = nullptr;
    PlayerEventCallback m_onPlayerLeave = nullptr;
};

} // namespace arena
