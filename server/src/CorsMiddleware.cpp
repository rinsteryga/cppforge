#include "server/CorsMiddleware.hpp"

#include <utility>

namespace cppforge::server
{
    CorsMiddleware::CorsMiddleware(std::string allowedOrigin) : m_allowedOrigin(std::move(allowedOrigin)) {}

    void CorsMiddleware::before_handle(crow::request &request, crow::response &response, context &)
    {
        applyHeaders(request, response);
    }

    void CorsMiddleware::after_handle(crow::request &request, crow::response &response, context &)
    {
        applyHeaders(request, response);
    }

    void CorsMiddleware::applyHeaders(const crow::request &request, crow::response &response) const
    {
        if (m_allowedOrigin.empty())
        {
            return;
        }

        const std::string origin(request.get_header_value("Origin"));
        if (origin.empty())
        {
            return;
        }

        response.set_header("Vary", "Origin");
        if (origin != m_allowedOrigin)
        {
            return;
        }

        response.set_header("Access-Control-Allow-Origin", origin);
        if (request.method == crow::HTTPMethod::Options)
        {
            response.set_header("Access-Control-Allow-Methods", "GET, OPTIONS");
            response.set_header("Access-Control-Allow-Headers", "Content-Type");
            response.set_header("Access-Control-Max-Age", "600");
        }
    }
} // namespace cppforge::server
