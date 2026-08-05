#include "controllers/TaskController.hpp"

#include <entities/TestCase.hpp>
#include <services/CodeRunner.hpp>

#include <string>

namespace cppforge::server::controllers
{
    void TaskController::registerRoutes(crow::SimpleApp &app)
    {
        CROW_ROUTE(app, "/api/v1/tasks/submit")
            .methods(crow::HTTPMethod::Post)(
                [](const crow::request &req)
                {
                    auto body = crow::json::load(req.body);
                    if (!body || !body.has("code"))
                    {
                        crow::json::wvalue res;
                        res["error"] = "Missing required 'code' field";
                        return crow::response(400, res);
                    }

                    std::string rawCode = std::string(body["code"].s());
                    QString code = QString::fromUtf8(rawCode.c_str());

                    std::vector<entities::TestCase> testCases;
                    if (body.has("tests") && body["tests"].t() == crow::json::type::List)
                    {
                        uint64_t id = 1;
                        for (const auto &item : body["tests"])
                        {
                            std::string rawInput = item.has("input") ? std::string(item["input"].s()) : "";
                            std::string rawExpected = item.has("expected") ? std::string(item["expected"].s()) : "";

                            QString input = QString::fromUtf8(rawInput.c_str());
                            QString expected = QString::fromUtf8(rawExpected.c_str());

                            testCases.emplace_back(id++, input, expected, false);
                        }
                    }

                    services::CodeRunner runner;
                    auto result = runner.runBlocking(code, testCases);

                    crow::json::wvalue res;
                    res["success"] = result.isSuccess();
                    res["output"] = result.getOutput().toStdString();
                    res["errors"] = result.getErrors().toStdString();
                    res["passed_tests_count"] = result.getPassedTestsCount();
                    res["total_tests_count"] = static_cast<uint32_t>(testCases.size());
                    res["using_docker"] = runner.isUsingDocker();

                    return crow::response(200, res);
                });
    }
} // namespace cppforge::server::controllers
