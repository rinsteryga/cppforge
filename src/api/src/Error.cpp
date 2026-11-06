#include "api/Error.hpp"

#include <nlohmann/json.hpp>

#include <utility>

namespace cppforge::api
{
    ErrorEnvelope makeError(std::string code, std::string message, nlohmann::json details)
    {
        return {{.code = std::move(code), .message = std::move(message), .details = std::move(details)}};
    }

    void to_json(nlohmann::json &json, const ApiError &error)
    {
        json = {{"code", error.code}, {"message", error.message}, {"details", error.details}};
    }

    void from_json(const nlohmann::json &json, ApiError &error)
    {
        json.at("code").get_to(error.code);
        json.at("message").get_to(error.message);
        error.details = json.value("details", nlohmann::json::object());
    }

    void to_json(nlohmann::json &json, const ErrorEnvelope &envelope)
    {
        json = {{"error", envelope.error}};
    }

    void from_json(const nlohmann::json &json, ErrorEnvelope &envelope)
    {
        json.at("error").get_to(envelope.error);
    }
} // namespace cppforge::api
