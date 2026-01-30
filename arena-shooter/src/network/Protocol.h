#pragma once

#include <cstdint>
#include "raylib.h"

namespace arena {

// Network constants
constexpr uint16_t DEFAULT_PORT = 7777;
constexpr int MAX_PACKET_SIZE = 1400;  // Safe MTU size
constexpr uint32_t PROTOCOL_ID = 0x41524E41; // "ARNA"
constexpr uint8_t PROTOCOL_VERSION = 1;
constexpr int PROTOCOL_MAX_PLAYERS = 16;

// Packet types
enum class PacketType : uint8_t {
    // Connection
    CONNECT_REQUEST = 0,
    CONNECT_ACCEPT,
    CONNECT_REJECT,
    DISCONNECT,
    HEARTBEAT,

    // Game state
    PLAYER_STATE,       // Server -> Client: Full player state
    WORLD_STATE,        // Server -> Client: All players snapshot

    // Player input
    PLAYER_INPUT,       // Client -> Server: Input commands

    // Game events
    PLAYER_SHOOT,
    PLAYER_HIT,
    PLAYER_DEATH,
    PLAYER_SPAWN,
    PLAYER_WEAPON_SWITCH,
    PLAYER_RELOAD,

    // Chat
    CHAT_MESSAGE,

    // Server info
    SERVER_INFO,
    PLAYER_LIST,
};

// Packet header (all packets start with this)
#pragma pack(push, 1)
struct PacketHeader {
    uint32_t protocolId;
    uint8_t version;
    PacketType type;
    uint16_t sequence;
    uint16_t ack;
    uint32_t ackBits;
};

// Connection packets
struct ConnectRequestPacket {
    PacketHeader header;
    char playerName[32];
};

struct ConnectAcceptPacket {
    PacketHeader header;
    uint8_t playerId;
    uint8_t maxPlayers;
    float tickRate;
};

struct ConnectRejectPacket {
    PacketHeader header;
    uint8_t reason;  // 0=full, 1=banned, 2=version mismatch
    char message[64];
};

// Player state (compact)
struct NetworkPlayerState {
    uint8_t playerId;
    float posX, posY, posZ;
    float velX, velY, velZ;
    int16_t yaw;            // Compressed: degrees * 100
    int16_t pitch;          // Compressed: degrees * 100
    uint8_t health;
    uint8_t weaponId;
    uint8_t flags;          // Bit flags: grounded, dead, reloading, etc.
    uint32_t lastInput;     // Last processed input sequence
};

struct WorldStatePacket {
    PacketHeader header;
    uint32_t serverTick;
    uint8_t playerCount;
    NetworkPlayerState players[PROTOCOL_MAX_PLAYERS];
};

// Player input
struct NetworkPlayerInput {
    float moveX, moveZ;     // Movement direction
    int16_t yaw;
    int16_t pitch;
    uint8_t buttons;        // Bit flags: jump, sprint, fire, reload, etc.
    uint8_t weaponSlot;
    uint32_t sequence;
};

struct PlayerInputPacket {
    PacketHeader header;
    NetworkPlayerInput input;
};

// Game events
struct PlayerShootPacket {
    PacketHeader header;
    uint8_t playerId;
    uint8_t weaponId;
    float originX, originY, originZ;
    float dirX, dirY, dirZ;
};

struct PlayerHitPacket {
    PacketHeader header;
    uint8_t targetId;
    uint8_t attackerId;
    uint8_t damage;
    float hitX, hitY, hitZ;
};

struct PlayerDeathPacket {
    PacketHeader header;
    uint8_t victimId;
    uint8_t killerId;
    uint8_t weaponId;
};

struct PlayerSpawnPacket {
    PacketHeader header;
    uint8_t playerId;
    float posX, posY, posZ;
    float yaw;
};

#pragma pack(pop)

// Button flags for input
enum InputButtons : uint8_t {
    INPUT_JUMP      = 1 << 0,
    INPUT_SPRINT    = 1 << 1,
    INPUT_FIRE      = 1 << 2,
    INPUT_ALT_FIRE  = 1 << 3,
    INPUT_RELOAD    = 1 << 4,
    INPUT_USE       = 1 << 5,
};

// Player state flags
enum PlayerFlags : uint8_t {
    FLAG_GROUNDED   = 1 << 0,
    FLAG_DEAD       = 1 << 1,
    FLAG_RELOADING  = 1 << 2,
    FLAG_SPRINTING  = 1 << 3,
};

} // namespace arena
