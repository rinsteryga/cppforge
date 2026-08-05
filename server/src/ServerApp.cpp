#include "ServerApp.hpp"

#include "controllers/AuthController.hpp"
#include "controllers/HealthController.hpp"
#include "controllers/TaskController.hpp"

#include <thread>

namespace cppforge::server
{
    ServerApp::ServerApp(uint16_t port) : m_port(port)
    {
        setupRoutes();
    }

    void ServerApp::setupRoutes()
    {
        controllers::HealthController::registerRoutes(m_app);
        controllers::AuthController::registerRoutes(m_app);
        controllers::TaskController::registerRoutes(m_app);
    }

    void ServerApp::run()
    {
        uint32_t concurrency = std::thread::hardware_concurrency();
        if (concurrency == 0)
        {
            concurrency = 4;
        }

        m_app.port(m_port).concurrency(concurrency).multithreaded().run();
    }
} // namespace cppforge::server
