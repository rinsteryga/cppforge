#include "server/controllers/ModulesController.hpp"

#include "server/HttpResponse.hpp"
#include <api/dto/ModuleDto.hpp>
#include <app/GetModuleById.hpp>
#include <app/GetModules.hpp>
#include <entities/Module.hpp>
#include <nlohmann/json.hpp>
#include <repositories/PgModuleRepository.hpp>

#include <QtSql/QSqlDatabase>

namespace cppforge::server::controllers
{
    void ModulesController::registerRoutes(HttpApp &app, data::DatabaseConnectionProvider &databaseProvider)
    {
        CROW_ROUTE(app, "/api/v1/modules")
            .methods(crow::HTTPMethod::Get)(
                [&databaseProvider]
                {
                    auto database = databaseProvider.connectionForCurrentThread();
                    repositories::PgModuleRepository repository(database);
                    cppforge::app::GetModules useCase(repository);
                    const auto modules = useCase.execute();
                    return jsonResponse(200, cppforge::api::toJson(cppforge::api::dto::toDto(modules)));
                });

        CROW_ROUTE(app, "/api/v1/modules/<uint>")
            .methods(crow::HTTPMethod::Get)(
                [&databaseProvider](std::uint64_t moduleId)
                {
                    auto database = databaseProvider.connectionForCurrentThread();
                    repositories::PgModuleRepository repository(database);
                    cppforge::app::GetModuleById useCase(repository);
                    const auto module = useCase.execute(moduleId);
                    if (!module)
                    {
                        return errorResponse(404, "NOT_FOUND", "Module not found", {{"id", moduleId}});
                    }
                    return jsonResponse(200, cppforge::api::toJson(cppforge::api::dto::toDto(*module)));
                });
    }
} // namespace cppforge::server::controllers
