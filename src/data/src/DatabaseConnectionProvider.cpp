#include "data/DatabaseConnectionProvider.hpp"

#include <QDebug>
#include <QString>
#include <QtSql/QSqlError>

#include <atomic>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace cppforge::data
{
    namespace
    {
        std::atomic<unsigned long long> &nextProviderId()
        {
            static std::atomic<unsigned long long> providerId{1};
            return providerId;
        }

        std::atomic<unsigned long long> &nextConnectionId()
        {
            static std::atomic<unsigned long long> connectionId{1};
            return connectionId;
        }

        QString toQString(const std::string &value)
        {
            return QString::fromUtf8(value.data(), static_cast<qsizetype>(value.size()));
        }

        struct ThreadConnection
        {
            ThreadConnection() = default;
            ThreadConnection(const ThreadConnection &) = delete;
            ThreadConnection &operator=(const ThreadConnection &) = delete;
            ThreadConnection(ThreadConnection &&) = delete;
            ThreadConnection &operator=(ThreadConnection &&) = delete;

            QSqlDatabase database;

            ~ThreadConnection()
            {
                if (!database.isValid())
                {
                    return;
                }

                const QString connectionName = database.connectionName();
                database.close();
                database = QSqlDatabase();
                QSqlDatabase::removeDatabase(connectionName);
            }
        };

        std::unordered_map<unsigned long long, ThreadConnection> &threadConnections()
        {
            thread_local std::unordered_map<unsigned long long, ThreadConnection> connections;
            return connections;
        }
    } // namespace

    DatabaseConnectionProvider::DatabaseConnectionProvider(DatabaseConfig config)
        : m_config(std::move(config)), m_providerId(nextProviderId().fetch_add(1))
    {
        if (m_config.host.empty() || m_config.database.empty() || m_config.user.empty() || m_config.port < 1 ||
            m_config.port > 65535)
        {
            throw std::invalid_argument("Invalid database configuration");
        }
    }

    DatabaseConnectionProvider::~DatabaseConnectionProvider() noexcept
    {
        try
        {
            threadConnections().erase(m_providerId);
        }
        catch (...)
        {
            qWarning() << "Failed to close the current thread's server database connection";
        }
    }

    QSqlDatabase DatabaseConnectionProvider::connectionForCurrentThread() const
    {
        auto &connections = threadConnections();
        auto &connection = connections[m_providerId];

        if (!connection.database.isValid())
        {
            const QString connectionName = QStringLiteral("cppforge_server_%1").arg(nextConnectionId().fetch_add(1));
            connection.database = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), connectionName);
            connection.database.setHostName(toQString(m_config.host));
            connection.database.setPort(m_config.port);
            connection.database.setDatabaseName(toQString(m_config.database));
            connection.database.setUserName(toQString(m_config.user));
            connection.database.setPassword(toQString(m_config.password));
        }

        if (!connection.database.isOpen() && !connection.database.open())
        {
            qWarning() << "Server database connection unavailable";
            throw std::runtime_error("Database connection unavailable");
        }

        return connection.database;
    }
} // namespace cppforge::data
