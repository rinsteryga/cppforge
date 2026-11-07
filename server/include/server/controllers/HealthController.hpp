#pragma once

#include "server/RequestLoggingMiddleware.hpp"
#include <data/DatabaseConnectionProvider.hpp>

namespace cppforge::server::controllers
{
    /**
     * @brief Registers liveness and database readiness endpoints.
     */
    class HealthController
    {
    public:
        /**
         * @brief Adds liveness and readiness routes.
         * @param app HTTP application.
         * @param databaseProvider Thread-safe server database provider.
         */
        static void registerRoutes(HttpApp &app, data::DatabaseConnectionProvider &databaseProvider);
    };
} // namespace cppforge::server::controllers
