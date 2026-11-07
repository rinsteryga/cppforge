#include "server/ServerConfig.hpp"

#include <QProcessEnvironment>
#include <QString>

#include <cctype>
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
                throw std::runtime_error("Required server configuration is missing: " + name.toStdString());
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
            const QString raw = environment.value(QStringLiteral("PG_PORT")).trimmed();
            if (raw.isEmpty())
            {
                throw std::runtime_error("Required server configuration is missing: PG_PORT");
            }

            bool valid = false;
            const int port = raw.toInt(&valid);
            if (!valid || port < 1 || port > 65535)
            {
                throw std::runtime_error("Invalid PG_PORT: expected an integer between 1 and 65535");
            }
            return port;
        }

        std::uint16_t serverPortValue(const QProcessEnvironment &environment)
        {
            QString raw = environment.value(QStringLiteral("SERVER_PORT"), QStringLiteral("18080")).trimmed();
            if (raw.isEmpty())
            {
                raw = QStringLiteral("18080");
            }

            bool valid = false;
            const uint port = raw.toUInt(&valid);
            if (!valid || port > 65535)
            {
                throw std::runtime_error("Invalid SERVER_PORT: expected an integer between 0 and 65535");
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
        for (auto &character : config.logLevel)
        {
            character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
        }
        config.allowedOrigin = optionalValue(environment, QStringLiteral("CORS_ALLOWED_ORIGIN"), QString());
        config.database.host = requiredValue(environment, QStringLiteral("PG_HOST"));
        config.database.port = portValue(environment);
        config.database.database = requiredValue(environment, QStringLiteral("PG_DB"));
        config.database.user = requiredValue(environment, QStringLiteral("PG_USER"));
        config.database.password = environment.value(QStringLiteral("PG_PASSWORD")).toStdString();
        return config;
    }
} // namespace cppforge::server
