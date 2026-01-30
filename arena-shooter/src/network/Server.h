#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <enet/enet.h>

#include "Protocol.h"
#include "game/Player.h"

namespace arena {

struct ClientConnection {
    ENetPeer* peer;
    uint8_t playerId;
    std::string playerName;
    uint32_t lastInputSequence;
    float lastHeartbeat;
    bool connected;
};

class Server {
public:
    Server();
    ~Server();

    // Lifecycle
    bool start(uint16_t port, int maxClients = MAX_PLAYERS);
    void stop();
    void update(float deltaTime);

    // State
    bool isRunning() const { return m_running; }
    int getPlayerCount() const;
    float getTickRate() const { return TICK_RATE; }

    // Send packets
    void broadcastWorldState();
    void sendToClient(uint8_t playerId, const void* data, size_t size,
                      bool reliable = false);
    void broadcastToAll(const void* data, size_t size, bool reliable = false);
    void broadcastExcept(uint8_t excludeId, const void* data, size_t size,
                         bool reliable = false);

private:
    ENetHost* m_host;
    bool m_running;

    std::unordered_map<uint8_t, ClientConnection> m_clients;
    std::vector<std::unique_ptr<Player>> m_players;

    uint16_t m_sequence;
    uint32_t m_tick;
    float m_tickAccumulator;

    // Event handling
    void handleConnect(ENetEvent& event);
    void handleDisconnect(ENetEvent& event);
    void handleReceive(ENetEvent& event);

    // Packet handlers
    void handleConnectRequest(ENetPeer* peer, const ConnectRequestPacket& packet);
    void handlePlayerInput(uint8_t playerId, const PlayerInputPacket& packet);

    // Game logic
    void simulateTick();
    void processPlayerInput(Player* player, const NetworkPlayerInput& input);
    void checkHits();

    // Utilities
    uint8_t findFreePlayerId() const;
    ClientConnection* getClientByPeer(ENetPeer* peer);
};

} // namespace arena
