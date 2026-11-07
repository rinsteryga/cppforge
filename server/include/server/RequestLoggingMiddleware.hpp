#pragma once

#include "server/CorsMiddleware.hpp"

#include <chrono>
#include <string>

#ifdef signals
#pragma push_macro("signals")
#undef signals
#include <crow.h>
#pragma pop_macro("signals")
#else
#include <crow.h>
#endif

namespace cppforge::server
{
    /**
     * @brief Adds request identifiers and concise access logging to HTTP requests.
     */
    class RequestLoggingMiddleware
    {
    public:
        struct context // NOLINT(readability-identifier-naming)
        {
            std::chrono::steady_clock::time_point startedAt;
            std::string requestId;
        };

        /** @brief Starts timing the request and assigns its request identifier. */
        void before_handle(
            crow::request &request, crow::response &response,
            context &ctx); // NOLINT(readability-identifier-naming, readability-convert-member-functions-to-static)

        /** @brief Adds the request identifier to the response and writes the access log. */
        void after_handle(
            crow::request &request, crow::response &response,
            context &ctx); // NOLINT(readability-identifier-naming, readability-convert-member-functions-to-static)
    };

    using HttpApp = crow::App<CorsMiddleware, RequestLoggingMiddleware>;
} // namespace cppforge::server
