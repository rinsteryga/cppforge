#pragma once

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
     * @brief Adds minimal CORS headers for one explicitly configured origin.
     *
     * The middleware is disabled when no origin is configured. It never emits a wildcard,
     * and preflight responses are completed by Crow before the after_handle chain runs, so
     * headers are applied in both before_handle and after_handle.
     */
    class CorsMiddleware
    {
    public:
        explicit CorsMiddleware(std::string allowedOrigin = {});

        struct context // NOLINT(readability-identifier-naming)
        {
        };

        void before_handle(crow::request &request, crow::response &response, context &ctx);
        void after_handle(crow::request &request, crow::response &response, context &ctx);

    private:
        std::string m_allowedOrigin;

        void applyHeaders(const crow::request &request, crow::response &response) const;
    };
} // namespace cppforge::server
