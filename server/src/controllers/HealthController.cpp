#include "controllers/HealthController.hpp"

#include <services/CodeRunner.hpp>

namespace cppforge::server::controllers
{
    void HealthController::registerRoutes(crow::SimpleApp &app)
    {
        CROW_ROUTE(app, "/api/v1/health")
        (
            []()
            {
                services::CodeRunner runner;
                crow::json::wvalue res;
                res["status"] = "ok";
                res["service"] = "cppforge_server";
                res["version"] = "1.0.3";
                res["docker_sandbox_active"] = runner.isUsingDocker();
                return crow::response(200, res);
            });
    }
} // namespace cppforge::server::controllers
