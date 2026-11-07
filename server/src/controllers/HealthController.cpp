#include "server/controllers/HealthController.hpp"

#include "server/HttpResponse.hpp"
#include <nlohmann/json.hpp>

#include <exception>

namespace cppforge::server::controllers
{
    void HealthController::registerRoutes(HttpApp &app, data::DatabaseConnectionProvider &databaseProvider)
    {
        CROW_ROUTE(app, "/api/v1/health")
            .methods(crow::HTTPMethod::Get)([] { return jsonResponse(200, nlohmann::json{{"status", "ok"}}.dump()); });

        CROW_ROUTE(app, "/api/v1/ready")
            .methods(crow::HTTPMethod::Get)(
                [&databaseProvider]
                {
                    try
                    {
                        databaseProvider.ensureHealthy();
                        return jsonResponse(200, nlohmann::json{{"status", "ready"}}.dump());
                    }
                    catch (const std::exception &)
                    {
                        return errorResponse(503, "DATABASE_UNAVAILABLE", "Database is not ready");
                    }
                });
    }
} // namespace cppforge::server::controllers
