#pragma once

#include <data/DatabaseConnectionProvider.hpp>

#include <cstdint>
#include <string>

namespace cppforge::server
{
    /**
     * @brief Runtime settings for the HTTP server and its database connection.
     */
    struct ServerConfig
    {
        std::string host{"127.0.0.1"};
        std::uint16_t port{18080};
        unsigned int concurrency{4};
        std::string logLevel{"info"};
        std::string allowedOrigin;
        data::DatabaseConfig database;

        /**
         * @brief Loads settings from process environment variables.
         * @return Validated configuration.
         * @throws std::runtime_error If required settings are missing or invalid.
         */
        static ServerConfig fromEnvironment();
    };
} // namespace cppforge::server
