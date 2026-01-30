# Arena Shooter

A 16-player multiplayer deathmatch game built with C++ and raylib.

## Features

- First-person shooter gameplay
- Up to 16 players in multiplayer
- Dedicated server architecture
- Two weapons: Rifle and Knife
- Arena-style map with cover and platforms

## Requirements

- CMake 3.14+
- C++17 compiler (GCC, Clang, or MSVC)
- OpenSCAD (optional, for asset generation)

### Linux Dependencies

Install the following development packages before building:

```bash
# Ubuntu/Debian
sudo apt-get install -y \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev

# Fedora/RHEL
sudo dnf install -y \
    libXrandr-devel \
    libXinerama-devel \
    libXcursor-devel \
    libXi-devel \
    mesa-libGL-devel

# Arch Linux
sudo pacman -S libxrandr libxinerama libxcursor libxi mesa
```

### macOS Dependencies

```bash
# Install Xcode command line tools (includes all required libraries)
xcode-select --install
```

### Windows Dependencies

On Windows, all dependencies are included via CMake FetchContent.

## Building

### Quick Build

```bash
./scripts/build.sh
```

### Manual Build

```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### Build Options

```bash
# Debug build
./scripts/build.sh debug

# Release build (default)
./scripts/build.sh release
```

## Running

### Client (Game)

```bash
./scripts/run_client.sh
# or
cd build && ./arena-shooter
```

### Dedicated Server

```bash
./scripts/run_server.sh [port] [max_players]
# or
cd build && ./arena-shooter-server --port 7777 --max-players 16
```

## Controls

| Key | Action |
|-----|--------|
| W/A/S/D | Move |
| Mouse | Look |
| Left Click | Fire |
| R | Reload |
| 1 | Rifle |
| 2 | Knife |
| Space | Jump |
| Shift | Sprint |
| Tab | Scoreboard |
| Escape | Menu |

## Project Structure

```
arena-shooter/
├── src/
│   ├── main.cpp              # Client entry point
│   ├── server_main.cpp       # Server entry point
│   ├── game/                 # Core game logic
│   │   ├── Game.cpp/.h
│   │   ├── Player.cpp/.h
│   │   ├── Weapon.cpp/.h
│   │   ├── Map.cpp/.h
│   │   └── Projectile.cpp/.h
│   ├── network/              # Networking
│   │   ├── Server.cpp/.h
│   │   ├── Client.cpp/.h
│   │   └── Protocol.h
│   ├── input/                # Input handling
│   │   └── InputHandler.cpp/.h
│   └── render/               # Rendering
│       └── Renderer.cpp/.h
├── assets/
│   ├── models/               # OpenSCAD source files
│   ├── textures/
│   └── sounds/
├── scripts/
│   ├── build.sh
│   ├── generate_assets.sh
│   ├── run_client.sh
│   └── run_server.sh
└── CMakeLists.txt
```

## Weapons

### Rifle
- Damage: 25 HP
- Fire Rate: 600 RPM
- Magazine: 30 rounds
- Range: 100m (hitscan)
- Reload: 2 seconds

### Knife
- Damage: 50 HP
- Range: 2m (melee)
- Attack Rate: 1 per second

## Network Protocol

The game uses ENet for reliable UDP networking with a 64 tick/s server.

- Client-side prediction for responsive movement
- Server reconciliation for authoritative game state
- Delta compression for efficient bandwidth usage

## Development Phases

- [x] Phase 1: Basic setup (window, 3D rendering, project structure)
- [ ] Phase 2: Singleplayer core (movement, weapons, map)
- [ ] Phase 3: Multiplayer (server/client, synchronization)
- [ ] Phase 4: Polish (HUD, sounds, effects)

## License

MIT License
