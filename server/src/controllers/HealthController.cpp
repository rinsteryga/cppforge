#include "server/controllers/HealthController.hpp"

#include "server/HttpResponse.hpp"
#include <nlohmann/json.hpp>

#include <QtSql/QSqlQuery>

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
                        auto database = databaseProvider.connectionForCurrentThread();
                        QSqlQuery query(database);
                        if (!query.exec("SELECT 1") || !query.next() || query.value(0).toInt() != 1)
                        {
                            return errorResponse(503, "DATABASE_UNAVAILABLE", "Database is not ready");
                        }
                        return jsonResponse(200, nlohmann::json{{"status", "ready"}}.dump());
                    }
                    catch (const std::exception &)
                    {
                        return errorResponse(503, "DATABASE_UNAVAILABLE", "Database is not ready");
                    }
                });
    }
} // namespace cppforge::server::controllers
