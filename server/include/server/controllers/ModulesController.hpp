#pragma once

#include "server/RequestLoggingMiddleware.hpp"
#include <data/DatabaseConnectionProvider.hpp>

namespace cppforge::server::controllers
{
    /**
     * @brief Registers module read endpoints backed by application use-cases.
     */
    class ModulesController
    {
    public:
        /**
         * @brief Adds module read routes backed by application use-cases.
         * @param app HTTP application.
         * @param databaseProvider Thread-safe server database provider.
         */
        static void registerRoutes(HttpApp &app, data::DatabaseConnectionProvider &databaseProvider);
    };
} // namespace cppforge::server::controllers
