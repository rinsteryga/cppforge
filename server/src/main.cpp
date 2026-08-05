#include "ServerApp.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    uint16_t port = 18080;
    if (argc > 1)
    {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    std::cout << "[CppForge Server] Starting High-Performance C++ REST API on port " << port << "..." << std::endl;

    try
    {
        cppforge::server::ServerApp server(port);
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "[CppForge Server] Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
