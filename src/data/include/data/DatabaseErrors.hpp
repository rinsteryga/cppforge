#pragma once

#include <stdexcept>
#include <string>

namespace cppforge::data
{
    /**
     * @brief Indicates that a database connection could not be established.
     *
     * The message is safe for server-side diagnostics but must not be exposed to API clients.
     */
    class DatabaseUnavailable : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    /**
     * @brief Indicates that a database query failed after a connection was established.
     */
    class QueryError : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
} // namespace cppforge::data
