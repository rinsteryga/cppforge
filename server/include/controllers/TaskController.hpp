#pragma once

#include "crow.h"

namespace cppforge::server::controllers
{
    /**
     * @brief Controller handling code evaluation and task endpoints (/api/v1/tasks/submit).
     */
    class TaskController
    {
    public:
        /**
         * @brief Registers task routes on the Crow application instance.
         * @param app Crow application reference.
         */
        static void registerRoutes(crow::SimpleApp &app);
    };
} // namespace cppforge::server::controllers
