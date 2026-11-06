#pragma once

#include <QtSql/QSqlDatabase>

#include <string>

namespace cppforge::data
{
    /**
     * @brief PostgreSQL connection settings independent of UI configuration.
     */
    struct DatabaseConfig
    {
        std::string host;
        int port{};
        std::string database;
        std::string user;
        std::string password;
    };

    /**
     * @brief Provides a dedicated Qt SQL connection to each calling thread.
     *
     * Each thread owns and uses its own named connection. The connection is closed and removed
     * when that thread exits; QSqlDatabase handles are never shared between threads.
     */
    class DatabaseConnectionProvider
    {
    public:
        /**
         * @brief Creates a provider with immutable connection settings.
         * @param config PostgreSQL connection settings.
         */
        explicit DatabaseConnectionProvider(DatabaseConfig config);

        /** @brief Closes the current thread's cached connection before provider destruction. */
        ~DatabaseConnectionProvider() noexcept;

        DatabaseConnectionProvider(const DatabaseConnectionProvider &) = delete;
        DatabaseConnectionProvider &operator=(const DatabaseConnectionProvider &) = delete;
        DatabaseConnectionProvider(DatabaseConnectionProvider &&) = delete;
        DatabaseConnectionProvider &operator=(DatabaseConnectionProvider &&) = delete;

        /**
         * @brief Returns the open connection associated with the current thread.
         * @return Thread-local PostgreSQL connection.
         * @throws std::runtime_error If the connection cannot be opened.
         */
        QSqlDatabase connectionForCurrentThread() const;

    private:
        DatabaseConfig m_config;
        unsigned long long m_providerId;
    };
} // namespace cppforge::data
