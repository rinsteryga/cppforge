#include "server/ServerApp.hpp"

#include "server/HttpResponse.hpp"
#include "server/controllers/HealthController.hpp"
#include "server/controllers/ModulesController.hpp"

#include <chrono>
#include <stdexcept>
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

    ServerApp::ServerApp(ServerConfig config) : m_config(std::move(config)), m_databaseProvider(m_config.database)
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
        m_app.port(m_config.port)
            .bindaddr(m_config.host)
            .concurrency(m_config.concurrency)
            .loglevel(crowLogLevel(m_config.logLevel));

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
        if (m_app.wait_for_server_start(std::chrono::seconds(5)) != std::cv_status::no_timeout || !m_app.is_bound())
        {
            m_app.stop();
            m_serverFuture.wait();
            throw std::runtime_error("HTTP server failed to bind");
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
