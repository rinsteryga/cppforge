#include "server/ServerApp.hpp"

#include "server/HttpResponse.hpp"
#include "server/controllers/HealthController.hpp"
#include "server/controllers/ModulesController.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <utility>

#include <crow.h>

namespace cppforge::server
{
    namespace
    {
        crow::LogLevel crowLogLevel(const std::string &level)
        {
            if (level == "debug")
            {
                return crow::LogLevel::Debug;
            }
            if (level == "warning" || level == "warn")
            {
                return crow::LogLevel::Warning;
            }
            if (level == "error")
            {
                return crow::LogLevel::Error;
            }
            if (level == "critical")
            {
                return crow::LogLevel::Critical;
            }
            return crow::LogLevel::Info;
        }
    } // namespace

    ServerApp::ServerApp(ServerConfig config, RouteConfigurator additionalRoutes)
        : m_config(std::move(config)), m_databaseProvider(m_config.database),
          m_app(CorsMiddleware(m_config.allowedOrigin),
                RequestLoggingMiddleware(crowLogLevel(m_config.logLevel) == crow::LogLevel::Info ||
                                         crowLogLevel(m_config.logLevel) == crow::LogLevel::Debug)),
          m_additionalRoutes(std::move(additionalRoutes))
    {
        configure();
        setupRoutes();
    }

    ServerApp::~ServerApp() noexcept
    {
        stop();
    }

    void ServerApp::configure()
    {
        auto applicationLogLevel = crowLogLevel(m_config.logLevel);
        if (applicationLogLevel == crow::LogLevel::Info)
        {
            // The middleware emits one structured access line; suppress Crow's duplicate
            // per-request Request/Response info logs while retaining warnings and errors.
            applicationLogLevel = crow::LogLevel::Warning;
        }

        m_app.port(m_config.port)
            .bindaddr(m_config.host)
            .concurrency(m_config.concurrency)
            .loglevel(applicationLogLevel);

        m_app.exception_handler(
            [](crow::response &response)
            {
                CROW_LOG_ERROR << "Unhandled request exception";
                response.code = 500;
                response.body = api::toJson(api::makeError("INTERNAL_ERROR", "An unexpected error occurred"));
                response.set_header("Content-Type", "application/json; charset=utf-8");
            });
    }

    void ServerApp::setupRoutes()
    {
        controllers::HealthController::registerRoutes(m_app, m_databaseProvider);
        controllers::ModulesController::registerRoutes(m_app, m_databaseProvider);
        registerCatchAllRoute();

        if (m_additionalRoutes)
        {
            m_additionalRoutes(m_app);
        }
    }

    void ServerApp::registerCatchAllRoute()
    {
        CROW_CATCHALL_ROUTE(m_app)(
            [](const crow::request &, crow::response &response)
            {
                if (response.code == 405)
                {
                    setErrorResponse(response, 405, "METHOD_NOT_ALLOWED", "Method not allowed");
                    return;
                }

                setErrorResponse(response, 404, "NOT_FOUND", "Resource not found");
            });
    }

    void ServerApp::run()
    {
        m_app.run();
    }

    void ServerApp::start()
    {
        if (m_serverFuture.valid())
        {
            throw std::logic_error("Server is already running");
        }

        m_serverFuture = m_app.run_async();

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        while (!m_app.is_bound() && std::chrono::steady_clock::now() < deadline)
        {
            if (m_serverFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (!m_app.is_bound())
        {
            std::string reason = "listener did not bind";
            m_app.stop();
            try
            {
                m_serverFuture.get();
            }
            catch (const std::exception &error)
            {
                reason = error.what();
            }
            catch (...)
            {
                reason = "unknown startup failure";
            }
            m_serverFuture = std::future<void>();
            CROW_LOG_ERROR << "HTTP server failed to start on " << m_config.host << ':' << m_config.port << ": "
                           << reason;
            throw std::runtime_error("Failed to bind HTTP server to " + m_config.host + ':' +
                                     std::to_string(m_config.port));
        }
    }

    void ServerApp::stop() noexcept // NOLINT(bugprone-exception-escape)
    {
        if (m_serverFuture.valid())
        {
            try
            {
                m_app.stop();
                m_serverFuture.wait();
            }
            catch (...)
            {
                CROW_LOG_ERROR << "Failed to stop HTTP server cleanly";
            }
            m_serverFuture = std::future<void>();
        }
    }

    std::uint16_t ServerApp::port() const
    {
        return m_app.port();
    }
} // namespace cppforge::server
