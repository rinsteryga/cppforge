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

        /**
         * @brief Closes the current thread's cached connection before provider destruction.
         *
         * The provider must be destroyed after all threads that used it have stopped. ServerApp
         * guarantees this by calling stop() in its destructor before member destruction.
         */
        ~DatabaseConnectionProvider() noexcept;

        DatabaseConnectionProvider(const DatabaseConnectionProvider &) = delete;
        DatabaseConnectionProvider &operator=(const DatabaseConnectionProvider &) = delete;
        DatabaseConnectionProvider(DatabaseConnectionProvider &&) = delete;
        DatabaseConnectionProvider &operator=(DatabaseConnectionProvider &&) = delete;

        /**
         * @brief Returns the open connection associated with the current thread.
         * @return Thread-local PostgreSQL connection.
         * @throws DatabaseUnavailable If the connection cannot be opened.
         */
        QSqlDatabase connectionForCurrentThread() const;

        /**
         * @brief Executes a lightweight health query on the current thread's connection.
         *
         * A failed check closes the cached connection so the next request retries opening it.
         * @return true when SELECT 1 succeeds, false otherwise.
         */
        bool ping() const;

        /**
         * @brief Ensures that the current thread's database connection is usable.
         * @throws DatabaseUnavailable If the health query fails.
         */
        void ensureHealthy() const;

    private:
        DatabaseConfig m_config;
        unsigned long long m_providerId;

        void resetCurrentThreadConnection() const;
    };
} // namespace cppforge::data
