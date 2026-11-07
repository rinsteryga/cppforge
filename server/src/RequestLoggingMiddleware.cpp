#include "server/RequestLoggingMiddleware.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <string>

namespace cppforge::server
{
    namespace
    {
        std::atomic<unsigned long long> &nextRequestId()
        {
            static std::atomic<unsigned long long> requestId{1};
            return requestId;
        }

        std::string methodName(crow::HTTPMethod method)
        {
            switch (method)
            {
            case crow::HTTPMethod::Get:
                return "GET";
            case crow::HTTPMethod::Post:
                return "POST";
            case crow::HTTPMethod::Put:
                return "PUT";
            case crow::HTTPMethod::Delete:
                return "DELETE";
            case crow::HTTPMethod::Patch:
                return "PATCH";
            case crow::HTTPMethod::Options:
                return "OPTIONS";
            default:
                return "UNKNOWN";
            }
        }

        std::string pathOnly(const std::string &url)
        {
            const auto queryStart = url.find('?');
            return url.substr(0, queryStart);
        }
    } // namespace

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void RequestLoggingMiddleware::before_handle(crow::request &request, crow::response &response,
                                                 context &ctx) // NOLINT(readability-convert-member-functions-to-static)
    {
        ctx.startedAt = std::chrono::steady_clock::now();
        ctx.requestId = "req-" + std::to_string(nextRequestId().fetch_add(1));
        response.set_header("X-Request-Id", ctx.requestId);
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void RequestLoggingMiddleware::after_handle(crow::request &request, crow::response &response,
                                                context &ctx) // NOLINT(readability-convert-member-functions-to-static)
    {
        if (ctx.requestId.empty())
        {
            // Crow completes unmatched OPTIONS requests before before_handle runs.
            ctx.requestId = "req-" + std::to_string(nextRequestId().fetch_add(1));
        }
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - ctx.startedAt);
        response.set_header("X-Request-Id", ctx.requestId);
        std::clog << "request_id=" << ctx.requestId << " method=" << methodName(request.method)
                  << " path=" << pathOnly(request.url) << " status=" << response.code
                  << " duration_ms=" << elapsed.count() << '\n';
    }
} // namespace cppforge::server
