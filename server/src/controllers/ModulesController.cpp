#include "server/controllers/ModulesController.hpp"

#include "server/HttpResponse.hpp"
#include <api/dto/ModuleDto.hpp>
#include <app/GetModuleById.hpp>
#include <app/GetModules.hpp>
#include <data/DatabaseErrors.hpp>
#include <entities/Module.hpp>
#include <nlohmann/json.hpp>
#include <repositories/PgModuleRepository.hpp>

#include <cstdint>
#include <limits>
#include <stdexcept>

namespace cppforge::server::controllers
{
    namespace
    {
        constexpr std::uint64_t maxPostgresBigint =
            static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());

        crow::response databaseFailureResponse()
        {
            return errorResponse(500, "INTERNAL_ERROR", "An unexpected error occurred");
        }
    } // namespace

    void ModulesController::registerRoutes(HttpApp &app, data::DatabaseConnectionProvider &databaseProvider)
    {
        CROW_ROUTE(app, "/api/v1/modules")
            .methods(crow::HTTPMethod::Get)(
                [&databaseProvider]
                {
                    try
                    {
                        auto database = databaseProvider.connectionForCurrentThread();
                        repositories::PgModuleRepository repository(database);
                        cppforge::app::GetModules useCase(repository);
                        const auto modules = useCase.execute();
                        return jsonResponse(200, cppforge::api::toJson(cppforge::api::dto::toDto(modules)));
                    }
                    catch (const data::DatabaseUnavailable &)
                    {
                        return errorResponse(503, "DATABASE_UNAVAILABLE", "Database is not ready");
                    }
                    catch (const std::exception &)
                    {
                        return databaseFailureResponse();
                    }
                });

        CROW_ROUTE(app, "/api/v1/modules/<uint>")
            .methods(crow::HTTPMethod::Get)(
                [&databaseProvider](std::uint64_t moduleId)
                {
                    if (moduleId > maxPostgresBigint)
                    {
                        return errorResponse(400, "INVALID_ARGUMENT", "Module id is out of range", {{"id", moduleId}});
                    }

                    try
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
                    }
                    catch (const data::DatabaseUnavailable &)
                    {
                        return errorResponse(503, "DATABASE_UNAVAILABLE", "Database is not ready");
                    }
                    catch (const std::exception &)
                    {
                        return databaseFailureResponse();
                    }
                });
    }
} // namespace cppforge::server::controllers
