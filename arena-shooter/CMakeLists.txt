cmake_minimum_required(VERSION 3.14)
project(arena-shooter VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# FetchContent for dependencies
include(FetchContent)

# Fetch raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

# Fetch ENet for networking
FetchContent_Declare(
    enet
    GIT_REPOSITORY https://github.com/lsalzman/enet.git
    GIT_TAG v1.3.18
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(enet)

# Source files
set(GAME_SOURCES
    src/main.cpp
    src/game/Game.cpp
    src/game/Player.cpp
    src/game/Weapon.cpp
    src/game/Map.cpp
    src/game/Projectile.cpp
    src/network/Server.cpp
    src/network/Client.cpp
    src/input/InputHandler.cpp
    src/render/Renderer.cpp
)

# Header files
set(GAME_HEADERS
    src/game/Game.h
    src/game/Player.h
    src/game/Weapon.h
    src/game/Map.h
    src/game/Projectile.h
    src/network/Server.h
    src/network/Client.h
    src/network/Protocol.h
    src/input/InputHandler.h
    src/render/Renderer.h
)

# Main executable
add_executable(${PROJECT_NAME} ${GAME_SOURCES} ${GAME_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    enet
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 winmm)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE m pthread)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Dedicated server executable (headless)
add_executable(${PROJECT_NAME}-server
    src/server_main.cpp
    src/game/Game.cpp
    src/game/Player.cpp
    src/game/Weapon.cpp
    src/game/Map.cpp
    src/game/Projectile.cpp
    src/network/Server.cpp
    src/network/Client.cpp
    src/input/InputHandler.cpp
    src/render/Renderer.cpp
)

target_include_directories(${PROJECT_NAME}-server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(${PROJECT_NAME}-server PRIVATE DEDICATED_SERVER)

target_link_libraries(${PROJECT_NAME}-server PRIVATE
    raylib
    enet
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME}-server PRIVATE ws2_32 winmm)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME}-server PRIVATE m pthread)
endif()
