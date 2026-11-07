#pragma once

#include <api/Error.hpp>
#include <api/Json.hpp>

#include <string>

#include <crow.h>

namespace cppforge::server
{
    /**
     * @brief Creates a JSON HTTP response.
     * @param status HTTP status code.
     * @param body Serialized JSON body.
     * @return Crow response with JSON content type.
     */
    inline crow::response jsonResponse(int status, const std::string &body)
    {
        crow::response response(status, body);
        response.set_header("Content-Type", "application/json; charset=utf-8");
        return response;
    }

    /**
     * @brief Replaces a response body with a client-safe API error envelope.
     * @param response Response to update in place.
     * @param status HTTP status code.
     * @param code Stable machine-readable error code.
     * @param message Human-readable error message.
     * @param details Optional JSON details.
     */
    inline void setErrorResponse(crow::response &response, int status, std::string code, std::string message,
                                 nlohmann::json details = nlohmann::json::object())
    {
        response.code = status;
        response.body = api::toJson(api::makeError(std::move(code), std::move(message), std::move(details)));
        response.set_header("Content-Type", "application/json; charset=utf-8");
    }

    /**
     * @brief Creates a client-safe API error response.
     * @param status HTTP status code.
     * @param code Stable machine-readable error code.
     * @param message Human-readable error message.
     * @param details Optional JSON details.
     * @return Error-envelope HTTP response.
     */
    inline crow::response errorResponse(int status, std::string code, std::string message,
                                        nlohmann::json details = nlohmann::json::object())
    {
        crow::response response;
        setErrorResponse(response, status, std::move(code), std::move(message), std::move(details));
        return response;
    }
} // namespace cppforge::server
