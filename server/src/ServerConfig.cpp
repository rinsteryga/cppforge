#include "server/ServerConfig.hpp"

#include <QProcessEnvironment>
#include <QString>

#include <stdexcept>

namespace cppforge::server
{
    namespace
    {
        std::string requiredValue(const QProcessEnvironment &environment, const QString &name)
        {
            const QString value = environment.value(name).trimmed();
            if (value.isEmpty())
            {
                throw std::runtime_error("Required server configuration is missing");
            }
            return value.toStdString();
        }

        std::string optionalValue(const QProcessEnvironment &environment, const QString &name, const QString &fallback)
        {
            const QString value = environment.value(name, fallback).trimmed();
            return (value.isEmpty() ? fallback : value).toStdString();
        }

        int portValue(const QProcessEnvironment &environment)
        {
            bool valid = false;
            const int port = environment.value(QStringLiteral("PG_PORT")).toInt(&valid);
            if (!valid || port < 1 || port > 65535)
            {
                throw std::runtime_error("Invalid database port configuration");
            }
            return port;
        }

        std::uint16_t serverPortValue(const QProcessEnvironment &environment)
        {
            bool valid = false;
            const uint port = environment.value(QStringLiteral("SERVER_PORT"), QStringLiteral("18080")).toUInt(&valid);
            if (!valid || port > 65535)
            {
                throw std::runtime_error("Invalid server port configuration");
            }
            return static_cast<std::uint16_t>(port);
        }
    } // namespace

    ServerConfig ServerConfig::fromEnvironment()
    {
        const auto environment = QProcessEnvironment::systemEnvironment();
        ServerConfig config;
        config.host = optionalValue(environment, QStringLiteral("SERVER_HOST"), QStringLiteral("127.0.0.1"));
        config.port = serverPortValue(environment);
        config.logLevel = optionalValue(environment, QStringLiteral("LOG_LEVEL"), QStringLiteral("info"));
        config.allowedOrigin = optionalValue(environment, QStringLiteral("CORS_ALLOWED_ORIGIN"), QString());
        config.database.host = requiredValue(environment, QStringLiteral("PG_HOST"));
        config.database.port = portValue(environment);
        config.database.database = requiredValue(environment, QStringLiteral("PG_DB"));
        config.database.user = requiredValue(environment, QStringLiteral("PG_USER"));
        config.database.password = environment.value(QStringLiteral("PG_PASSWORD")).toStdString();
        return config;
    }
} // namespace cppforge::server
