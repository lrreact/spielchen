#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

#include "network/Server.h"
#include "game/Game.h"

std::atomic<bool> running(true);

void signalHandler(int signum) {
    std::cout << "\nShutting down server..." << std::endl;
    running = false;
}

int main(int argc, char* argv[])
{
    std::cout << "==================================" << std::endl;
    std::cout << "  Arena Shooter Dedicated Server  " << std::endl;
    std::cout << "==================================" << std::endl;

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Parse command line arguments
    int port = 7777;
    int maxPlayers = 16;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                port = std::atoi(argv[++i]);
            }
        } else if (arg == "-m" || arg == "--max-players") {
            if (i + 1 < argc) {
                maxPlayers = std::atoi(argv[++i]);
            }
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -p, --port <port>        Server port (default: 7777)" << std::endl;
            std::cout << "  -m, --max-players <num>  Max players (default: 16)" << std::endl;
            std::cout << "  -h, --help               Show this help" << std::endl;
            return 0;
        }
    }

    std::cout << "Starting server on port " << port << std::endl;
    std::cout << "Max players: " << maxPlayers << std::endl;

    // TODO: Initialize and run server
    // Server server(port, maxPlayers);
    // server.run();

    std::cout << "Server placeholder - Phase 1 complete" << std::endl;
    std::cout << "Full server implementation coming in Phase 3" << std::endl;

    // Temporary: just wait for signal
    while (running) {
        // Server tick would go here
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 ticks/sec
    }

    std::cout << "Server shutdown complete." << std::endl;

    return 0;
}
