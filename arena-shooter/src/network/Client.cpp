#include "Client.h"
#include <iostream>
#include <cstring>

namespace arena {

Client::Client()
    : m_client(nullptr)
    , m_server(nullptr)
    , m_state(ConnectionState::DISCONNECTED)
    , m_localPlayerId(255)
    , m_ping(0.0f)
    , m_sequence(0)
    , m_lastAck(0)
    , m_ackBits(0)
    , m_lastAckedInput(0)
{
}

Client::~Client()
{
    disconnect();
}

bool Client::connect(const std::string& host, uint16_t port,
                     const std::string& playerName)
{
    if (m_state != ConnectionState::DISCONNECTED) {
        return false;
    }

    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet" << std::endl;
        return false;
    }

    m_client = enet_host_create(nullptr, 1, 2, 0, 0);

    if (!m_client) {
        std::cerr << "Failed to create ENet client host" << std::endl;
        enet_deinitialize();
        return false;
    }

    ENetAddress address;
    enet_address_set_host(&address, host.c_str());
    address.port = port;

    m_server = enet_host_connect(m_client, &address, 2, 0);

    if (!m_server) {
        std::cerr << "Failed to initiate connection" << std::endl;
        enet_host_destroy(m_client);
        m_client = nullptr;
        enet_deinitialize();
        return false;
    }

    m_playerName = playerName;
    m_state = ConnectionState::CONNECTING;

    // Wait for connection
    ENetEvent event;
    if (enet_host_service(m_client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {

        // Send connect request
        ConnectRequestPacket request;
        std::memset(&request, 0, sizeof(request));
        request.header.protocolId = PROTOCOL_ID;
        request.header.version = PROTOCOL_VERSION;
        request.header.type = PacketType::CONNECT_REQUEST;
        std::strncpy(request.playerName, playerName.c_str(),
            sizeof(request.playerName) - 1);

        ENetPacket* packet = enet_packet_create(&request, sizeof(request),
            ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(m_server, 0, packet);
        enet_host_flush(m_client);

        std::cout << "Connected to server, sending join request..." << std::endl;
        return true;
    }

    std::cerr << "Failed to connect to server" << std::endl;
    enet_peer_reset(m_server);
    enet_host_destroy(m_client);
    m_client = nullptr;
    m_server = nullptr;
    m_state = ConnectionState::DISCONNECTED;
    enet_deinitialize();

    return false;
}

void Client::disconnect()
{
    if (m_state == ConnectionState::DISCONNECTED) {
        return;
    }

    m_state = ConnectionState::DISCONNECTING;

    if (m_server) {
        enet_peer_disconnect(m_server, 0);

        // Wait for disconnect
        ENetEvent event;
        while (enet_host_service(m_client, &event, 3000) > 0) {
            if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
                break;
            } else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
                enet_packet_destroy(event.packet);
            }
        }

        enet_peer_reset(m_server);
        m_server = nullptr;
    }

    if (m_client) {
        enet_host_destroy(m_client);
        m_client = nullptr;
    }

    enet_deinitialize();

    m_state = ConnectionState::DISCONNECTED;
    m_localPlayerId = 255;

    std::cout << "Disconnected from server" << std::endl;
}

void Client::update(float deltaTime)
{
    if (!m_client) {
        return;
    }

    ENetEvent event;
    while (enet_host_service(m_client, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                handleReceive(event);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Disconnected from server" << std::endl;
                m_state = ConnectionState::DISCONNECTED;
                m_server = nullptr;
                break;

            default:
                break;
        }
    }

    // Update ping
    if (m_server) {
        m_ping = m_server->roundTripTime;
    }
}

void Client::sendInput(const PlayerInput& input)
{
    if (m_state != ConnectionState::CONNECTED || !m_server) {
        return;
    }

    PlayerInputPacket packet;
    std::memset(&packet, 0, sizeof(packet));

    packet.header.protocolId = PROTOCOL_ID;
    packet.header.version = PROTOCOL_VERSION;
    packet.header.type = PacketType::PLAYER_INPUT;
    packet.header.sequence = m_sequence++;

    packet.input.moveX = input.moveDirection.x;
    packet.input.moveZ = input.moveDirection.z;
    packet.input.yaw = static_cast<int16_t>(input.lookDelta.x * 100);
    packet.input.pitch = static_cast<int16_t>(input.lookDelta.y * 100);
    packet.input.sequence = input.sequence;

    uint8_t buttons = 0;
    if (input.jump) buttons |= INPUT_JUMP;
    if (input.sprint) buttons |= INPUT_SPRINT;
    if (input.primaryFire) buttons |= INPUT_FIRE;
    if (input.reload) buttons |= INPUT_RELOAD;
    packet.input.buttons = buttons;
    packet.input.weaponSlot = static_cast<uint8_t>(input.weaponSlot);

    // Store for reconciliation
    m_inputHistory.push(packet.input);
    while (m_inputHistory.size() > INPUT_BUFFER_SIZE) {
        m_inputHistory.pop();
    }

    ENetPacket* pkt = enet_packet_create(&packet, sizeof(packet), 0);
    enet_peer_send(m_server, 1, pkt);
}

void Client::handleReceive(ENetEvent& event)
{
    if (event.packet->dataLength < sizeof(PacketHeader)) {
        return;
    }

    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(event.packet->data);

    if (header->protocolId != PROTOCOL_ID) {
        return;
    }

    switch (header->type) {
        case PacketType::CONNECT_ACCEPT:
            if (event.packet->dataLength >= sizeof(ConnectAcceptPacket)) {
                handleConnectAccept(
                    *reinterpret_cast<const ConnectAcceptPacket*>(event.packet->data));
            }
            break;

        case PacketType::CONNECT_REJECT:
            if (event.packet->dataLength >= sizeof(ConnectRejectPacket)) {
                handleConnectReject(
                    *reinterpret_cast<const ConnectRejectPacket*>(event.packet->data));
            }
            break;

        case PacketType::WORLD_STATE:
            if (event.packet->dataLength >= sizeof(WorldStatePacket)) {
                handleWorldState(
                    *reinterpret_cast<const WorldStatePacket*>(event.packet->data));
            }
            break;

        case PacketType::PLAYER_HIT:
            if (event.packet->dataLength >= sizeof(PlayerHitPacket)) {
                handlePlayerHit(
                    *reinterpret_cast<const PlayerHitPacket*>(event.packet->data));
            }
            break;

        case PacketType::PLAYER_DEATH:
            if (event.packet->dataLength >= sizeof(PlayerDeathPacket)) {
                handlePlayerDeath(
                    *reinterpret_cast<const PlayerDeathPacket*>(event.packet->data));
            }
            break;

        case PacketType::PLAYER_SPAWN:
            if (event.packet->dataLength >= sizeof(PlayerSpawnPacket)) {
                handlePlayerSpawn(
                    *reinterpret_cast<const PlayerSpawnPacket*>(event.packet->data));
            }
            break;

        default:
            break;
    }
}

void Client::handleConnectAccept(const ConnectAcceptPacket& packet)
{
    m_localPlayerId = packet.playerId;
    m_state = ConnectionState::CONNECTED;

    std::cout << "Joined server as player " << static_cast<int>(m_localPlayerId)
              << " (max: " << static_cast<int>(packet.maxPlayers) << " players, "
              << packet.tickRate << " tick/s)" << std::endl;
}

void Client::handleConnectReject(const ConnectRejectPacket& packet)
{
    std::cerr << "Connection rejected: " << packet.message << std::endl;
    m_state = ConnectionState::DISCONNECTED;
}

void Client::handleWorldState(const WorldStatePacket& packet)
{
    if (m_onWorldState) {
        m_onWorldState(packet);
    }

    // Find our player state and reconcile
    for (int i = 0; i < packet.playerCount; i++) {
        if (packet.players[i].playerId == m_localPlayerId) {
            reconcileState(packet.players[i]);
            break;
        }
    }
}

void Client::handlePlayerHit(const PlayerHitPacket& packet)
{
    // TODO: Play hit effects
}

void Client::handlePlayerDeath(const PlayerDeathPacket& packet)
{
    // TODO: Handle death events
}

void Client::handlePlayerSpawn(const PlayerSpawnPacket& packet)
{
    // TODO: Handle spawn events
}

void Client::reconcileState(const NetworkPlayerState& serverState)
{
    m_lastAckedInput = serverState.lastInput;

    // Remove acknowledged inputs from history
    while (!m_inputHistory.empty() &&
           m_inputHistory.front().sequence <= m_lastAckedInput) {
        m_inputHistory.pop();
    }

    // TODO: Re-apply unacknowledged inputs for proper reconciliation
}

} // namespace arena
