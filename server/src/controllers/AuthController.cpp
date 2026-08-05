#include "controllers/AuthController.hpp"

#include <services/AuthManager.hpp>

namespace cppforge::server::controllers
{
    void AuthController::registerRoutes(crow::SimpleApp &app)
    {
        CROW_ROUTE(app, "/api/v1/auth/login")
            .methods(crow::HTTPMethod::Post)(
                [](const crow::request &req)
                {
                    auto body = crow::json::load(req.body);
                    if (!body || !body.has("email") || !body.has("password"))
                    {
                        crow::json::wvalue res;
                        res["error"] = "Missing required fields: email, password";
                        return crow::response(400, res);
                    }

                    std::string email = body["email"].s();
                    std::string password = body["password"].s();

                    crow::json::wvalue res;
                    res["message"] = "Login endpoint reached";
                    res["email"] = email;
                    res["status"] = "success";
                    return crow::response(200, res);
                });

        CROW_ROUTE(app, "/api/v1/auth/signup")
            .methods(crow::HTTPMethod::Post)(
                [](const crow::request &req)
                {
                    auto body = crow::json::load(req.body);
                    if (!body || !body.has("username") || !body.has("email") || !body.has("password"))
                    {
                        crow::json::wvalue res;
                        res["error"] = "Missing required fields: username, email, password";
                        return crow::response(400, res);
                    }

                    std::string username = body["username"].s();
                    std::string email = body["email"].s();

                    crow::json::wvalue res;
                    res["message"] = "Registration successful";
                    res["username"] = username;
                    res["email"] = email;
                    res["status"] = "created";
                    return crow::response(201, res);
                });
    }
} // namespace cppforge::server::controllers
