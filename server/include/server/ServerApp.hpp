#pragma once

#include "server/RequestLoggingMiddleware.hpp"
#include "server/ServerConfig.hpp"
#include <data/DatabaseConnectionProvider.hpp>

#include <future>

namespace cppforge::server
{
    /**
     * @brief Configures and runs the CppForge HTTP API server.
     */
    class ServerApp
    {
    public:
        /**
         * @brief Constructs the server and its per-thread database connection provider.
         * @param config Server and database configuration.
         */
        explicit ServerApp(ServerConfig config);
        ~ServerApp() noexcept;

        ServerApp(const ServerApp &) = delete;
        ServerApp &operator=(const ServerApp &) = delete;
        ServerApp(ServerApp &&) = delete;
        ServerApp &operator=(ServerApp &&) = delete;

        /**
         * @brief Runs the HTTP event loop on the calling thread.
         */
        void run();

        /**
         * @brief Starts the HTTP server asynchronously for embedding and integration tests.
         * @throws std::runtime_error If the listener cannot start.
         */
        void start();

        /**
         * @brief Stops the server and waits for its worker threads to exit.
         */
        void stop() noexcept;

        /**
         * @brief Returns the bound port, including the selected port when configured with zero.
         * @return Listening TCP port.
         */
        std::uint16_t port() const;

    private:
        ServerConfig m_config;
        data::DatabaseConnectionProvider m_databaseProvider;
        HttpApp m_app;
        std::future<void> m_serverFuture;

        void configure();
        void setupRoutes();
    };
} // namespace cppforge::server
