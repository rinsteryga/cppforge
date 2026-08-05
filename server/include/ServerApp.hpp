#pragma once

#include "crow.h"

namespace cppforge::server
{
    /**
     * @brief Main Server Application orchestrator configuring Crow routes and server lifecycle.
     */
    class ServerApp
    {
    public:
        /**
         * @brief Constructs ServerApp instance listening on specified port.
         * @param port Target HTTP port (default: 18080).
         */
        explicit ServerApp(uint16_t port = 18080);

        /**
         * @brief Starts the server event loop.
         */
        void run();

    private:
        uint16_t m_port;
        crow::SimpleApp m_app;

        void setupRoutes();
    };
} // namespace cppforge::server
