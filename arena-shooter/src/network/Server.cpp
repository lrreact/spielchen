#include "Server.h"
#include <iostream>
#include <cstring>

namespace arena {

Server::Server()
    : m_host(nullptr)
    , m_running(false)
    , m_sequence(0)
    , m_tick(0)
    , m_tickAccumulator(0.0f)
{
}

Server::~Server()
{
    stop();
}

bool Server::start(uint16_t port, int maxClients)
{
    if (m_running) {
        return false;
    }

    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet" << std::endl;
        return false;
    }

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    m_host = enet_host_create(&address, maxClients, 2, 0, 0);

    if (!m_host) {
        std::cerr << "Failed to create ENet server host" << std::endl;
        enet_deinitialize();
        return false;
    }

    m_running = true;
    std::cout << "Server started on port " << port << std::endl;

    return true;
}

void Server::stop()
{
    if (!m_running) {
        return;
    }

    // Disconnect all clients
    for (auto& [id, client] : m_clients) {
        if (client.peer) {
            enet_peer_disconnect(client.peer, 0);
        }
    }

    // Wait briefly for disconnects
    ENetEvent event;
    while (enet_host_service(m_host, &event, 1000) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            enet_packet_destroy(event.packet);
        }
    }

    enet_host_destroy(m_host);
    m_host = nullptr;

    enet_deinitialize();

    m_running = false;
    m_clients.clear();
    m_players.clear();

    std::cout << "Server stopped" << std::endl;
}

void Server::update(float deltaTime)
{
    if (!m_running) {
        return;
    }

    // Process network events
    ENetEvent event;
    while (enet_host_service(m_host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                handleConnect(event);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                handleDisconnect(event);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                handleReceive(event);
                enet_packet_destroy(event.packet);
                break;

            default:
                break;
        }
    }

    // Fixed timestep game simulation
    m_tickAccumulator += deltaTime;

    while (m_tickAccumulator >= TICK_INTERVAL) {
        simulateTick();
        m_tickAccumulator -= TICK_INTERVAL;
        m_tick++;

        // Send world state every tick
        broadcastWorldState();
    }
}

int Server::getPlayerCount() const
{
    return static_cast<int>(m_clients.size());
}

void Server::broadcastWorldState()
{
    WorldStatePacket packet;
    std::memset(&packet, 0, sizeof(packet));

    packet.header.protocolId = PROTOCOL_ID;
    packet.header.version = PROTOCOL_VERSION;
    packet.header.type = PacketType::WORLD_STATE;
    packet.header.sequence = m_sequence++;

    packet.serverTick = m_tick;
    packet.playerCount = static_cast<uint8_t>(m_players.size());

    int idx = 0;
    for (const auto& player : m_players) {
        if (!player || idx >= MAX_PLAYERS) break;

        const PlayerState& state = player->getState();

        packet.players[idx].playerId = player->getId();
        packet.players[idx].posX = state.position.x;
        packet.players[idx].posY = state.position.y;
        packet.players[idx].posZ = state.position.z;
        packet.players[idx].velX = state.velocity.x;
        packet.players[idx].velY = state.velocity.y;
        packet.players[idx].velZ = state.velocity.z;
        packet.players[idx].yaw = static_cast<int16_t>(state.yaw * 100);
        packet.players[idx].pitch = static_cast<int16_t>(state.pitch * 100);
        packet.players[idx].health = static_cast<uint8_t>(state.health);
        packet.players[idx].weaponId = static_cast<uint8_t>(state.currentWeapon);
        packet.players[idx].lastInput = state.lastProcessedInput;

        uint8_t flags = 0;
        if (state.isGrounded) flags |= FLAG_GROUNDED;
        if (state.isDead) flags |= FLAG_DEAD;
        packet.players[idx].flags = flags;

        idx++;
    }

    broadcastToAll(&packet, sizeof(packet), false);
}

void Server::sendToClient(uint8_t playerId, const void* data, size_t size,
                          bool reliable)
{
    auto it = m_clients.find(playerId);
    if (it == m_clients.end() || !it->second.peer) {
        return;
    }

    ENetPacket* packet = enet_packet_create(
        data, size,
        reliable ? ENET_PACKET_FLAG_RELIABLE : 0
    );

    enet_peer_send(it->second.peer, reliable ? 0 : 1, packet);
}

void Server::broadcastToAll(const void* data, size_t size, bool reliable)
{
    ENetPacket* packet = enet_packet_create(
        data, size,
        reliable ? ENET_PACKET_FLAG_RELIABLE : 0
    );

    enet_host_broadcast(m_host, reliable ? 0 : 1, packet);
}

void Server::broadcastExcept(uint8_t excludeId, const void* data, size_t size,
                             bool reliable)
{
    for (auto& [id, client] : m_clients) {
        if (id != excludeId && client.peer) {
            ENetPacket* packet = enet_packet_create(
                data, size,
                reliable ? ENET_PACKET_FLAG_RELIABLE : 0
            );
            enet_peer_send(client.peer, reliable ? 0 : 1, packet);
        }
    }
}

void Server::handleConnect(ENetEvent& event)
{
    std::cout << "Client connecting..." << std::endl;
    // Wait for connect request packet
    event.peer->data = nullptr;
}

void Server::handleDisconnect(ENetEvent& event)
{
    ClientConnection* client = getClientByPeer(event.peer);
    if (client) {
        std::cout << "Player " << client->playerName << " disconnected" << std::endl;

        // Remove player
        m_players.erase(
            std::remove_if(m_players.begin(), m_players.end(),
                [client](const auto& p) { return p && p->getId() == client->playerId; }),
            m_players.end()
        );

        m_clients.erase(client->playerId);
    }
}

void Server::handleReceive(ENetEvent& event)
{
    if (event.packet->dataLength < sizeof(PacketHeader)) {
        return;
    }

    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(event.packet->data);

    if (header->protocolId != PROTOCOL_ID) {
        return;
    }

    switch (header->type) {
        case PacketType::CONNECT_REQUEST:
            if (event.packet->dataLength >= sizeof(ConnectRequestPacket)) {
                handleConnectRequest(event.peer,
                    *reinterpret_cast<const ConnectRequestPacket*>(event.packet->data));
            }
            break;

        case PacketType::PLAYER_INPUT: {
            ClientConnection* client = getClientByPeer(event.peer);
            if (client && event.packet->dataLength >= sizeof(PlayerInputPacket)) {
                handlePlayerInput(client->playerId,
                    *reinterpret_cast<const PlayerInputPacket*>(event.packet->data));
            }
            break;
        }

        default:
            break;
    }
}

void Server::handleConnectRequest(ENetPeer* peer, const ConnectRequestPacket& packet)
{
    // Check if server is full
    if (m_clients.size() >= MAX_PLAYERS) {
        ConnectRejectPacket reject;
        reject.header.protocolId = PROTOCOL_ID;
        reject.header.version = PROTOCOL_VERSION;
        reject.header.type = PacketType::CONNECT_REJECT;
        reject.reason = 0; // Full
        std::strncpy(reject.message, "Server is full", sizeof(reject.message));

        ENetPacket* pkt = enet_packet_create(&reject, sizeof(reject),
            ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, pkt);

        enet_peer_disconnect_later(peer, 0);
        return;
    }

    // Create new client
    uint8_t playerId = findFreePlayerId();

    ClientConnection client;
    client.peer = peer;
    client.playerId = playerId;
    client.playerName = packet.playerName;
    client.lastInputSequence = 0;
    client.lastHeartbeat = 0.0f;
    client.connected = true;

    m_clients[playerId] = client;
    peer->data = reinterpret_cast<void*>(static_cast<uintptr_t>(playerId));

    // Create player
    auto player = std::make_unique<Player>(playerId, client.playerName);
    // TODO: Set spawn position
    m_players.push_back(std::move(player));

    // Send accept packet
    ConnectAcceptPacket accept;
    accept.header.protocolId = PROTOCOL_ID;
    accept.header.version = PROTOCOL_VERSION;
    accept.header.type = PacketType::CONNECT_ACCEPT;
    accept.playerId = playerId;
    accept.maxPlayers = MAX_PLAYERS;
    accept.tickRate = TICK_RATE;

    ENetPacket* pkt = enet_packet_create(&accept, sizeof(accept),
        ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, pkt);

    std::cout << "Player " << client.playerName << " joined (ID: "
              << static_cast<int>(playerId) << ")" << std::endl;
}

void Server::handlePlayerInput(uint8_t playerId, const PlayerInputPacket& packet)
{
    // Find player
    Player* player = nullptr;
    for (auto& p : m_players) {
        if (p && p->getId() == playerId) {
            player = p.get();
            break;
        }
    }

    if (!player) return;

    processPlayerInput(player, packet.input);

    auto it = m_clients.find(playerId);
    if (it != m_clients.end()) {
        it->second.lastInputSequence = packet.input.sequence;
    }
}

void Server::simulateTick()
{
    // Update all players
    for (auto& player : m_players) {
        if (player) {
            player->update(TICK_INTERVAL);
        }
    }

    // Check hits
    checkHits();
}

void Server::processPlayerInput(Player* player, const NetworkPlayerInput& input)
{
    PlayerInput playerInput;
    playerInput.moveDirection = { input.moveX, 0.0f, input.moveZ };
    playerInput.lookDelta = { 0.0f, 0.0f }; // Rotation sent directly
    playerInput.jump = (input.buttons & INPUT_JUMP) != 0;
    playerInput.sprint = (input.buttons & INPUT_SPRINT) != 0;
    playerInput.primaryFire = (input.buttons & INPUT_FIRE) != 0;
    playerInput.reload = (input.buttons & INPUT_RELOAD) != 0;
    playerInput.weaponSlot = input.weaponSlot;
    playerInput.sequence = input.sequence;

    // Apply rotation directly
    player->getState().yaw = input.yaw / 100.0f;
    player->getState().pitch = input.pitch / 100.0f;

    player->processInput(playerInput, TICK_INTERVAL);
}

void Server::checkHits()
{
    // TODO: Implement server-side hit detection
}

uint8_t Server::findFreePlayerId() const
{
    for (uint8_t id = 0; id < MAX_PLAYERS; id++) {
        if (m_clients.find(id) == m_clients.end()) {
            return id;
        }
    }
    return 255; // Invalid
}

ClientConnection* Server::getClientByPeer(ENetPeer* peer)
{
    if (!peer->data) return nullptr;

    uint8_t playerId = static_cast<uint8_t>(reinterpret_cast<uintptr_t>(peer->data));
    auto it = m_clients.find(playerId);

    if (it != m_clients.end()) {
        return &it->second;
    }

    return nullptr;
}

} // namespace arena
