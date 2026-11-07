#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace cppforge::api
{
    /**
     * @brief Serializes an API value to compact UTF-8 JSON.
     * @tparam T Serializable API value.
     * @param value Value to serialize.
     * @return Compact JSON string.
     */
    template <typename T> std::string toJson(const T &value)
    {
        return nlohmann::json(value).dump();
    }
} // namespace cppforge::api
