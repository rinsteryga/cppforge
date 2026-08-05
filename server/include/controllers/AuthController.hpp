#pragma once

#include "crow.h"

namespace cppforge::server::controllers
{
    /**
     * @brief Controller handling REST authentication endpoints (/api/v1/auth/login, /api/v1/auth/signup).
     */
    class AuthController
    {
    public:
        /**
         * @brief Registers authentication routes on the Crow application instance.
         * @param app Crow application reference.
         */
        static void registerRoutes(crow::SimpleApp &app);
    };
} // namespace cppforge::server::controllers
