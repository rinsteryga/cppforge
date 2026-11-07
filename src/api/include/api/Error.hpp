#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace cppforge::api
{
    /**
     * @brief Describes a stable, client-safe API error.
     */
    struct ApiError
    {
        std::string code;
        std::string message;
        nlohmann::json details = nlohmann::json::object();
    };

    /**
     * @brief Standard error response envelope.
     */
    struct ErrorEnvelope
    {
        ApiError error;
    };

    /**
     * @brief Creates an error envelope with client-safe code, message, and details.
     * @param code Stable machine-readable error code.
     * @param message Human-readable message safe for clients.
     * @param details Optional field-level details.
     * @return Standard error envelope.
     */
    ErrorEnvelope makeError(std::string code, std::string message, nlohmann::json details = nlohmann::json::object());

    /** @brief Serializes the inner error object. */
    void to_json(nlohmann::json &json, const ApiError &error); // NOLINT(readability-identifier-naming)

    /** @brief Parses the inner error object. */
    void from_json(const nlohmann::json &json, ApiError &error); // NOLINT(readability-identifier-naming)

    /** @brief Serializes the standard error envelope. */
    void to_json(nlohmann::json &json, const ErrorEnvelope &envelope); // NOLINT(readability-identifier-naming)

    /** @brief Parses the standard error envelope. */
    void from_json(const nlohmann::json &json, ErrorEnvelope &envelope); // NOLINT(readability-identifier-naming)
} // namespace cppforge::api
