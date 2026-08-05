#pragma once

#include "crow.h"

namespace cppforge::server::controllers
{
    /**
     * @brief Controller handling server health checks and system diagnostics.
     */
    class HealthController
    {
    public:
        /**
         * @brief Registers health check routes on the Crow application instance.
         * @param app Crow application reference.
         */
        static void registerRoutes(crow::SimpleApp &app);
    };
} // namespace cppforge::server::controllers
