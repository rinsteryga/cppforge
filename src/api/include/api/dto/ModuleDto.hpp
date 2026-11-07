#pragma once

#include <nlohmann/json_fwd.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace cppforge::entities
{
    class Module;
}

namespace cppforge::api::dto
{
    /**
     * @brief Public API representation of a learning module.
     */
    struct ModuleDto
    {
        std::uint64_t id{};
        std::string name;
        std::string description;
    };

    /**
     * @brief Pagination metadata for a module collection.
     */
    struct ModuleListMeta
    {
        std::size_t total{};
    };

    /**
     * @brief Standard response shape for a module collection.
     */
    struct ModuleListDto
    {
        std::vector<ModuleDto> items;
        ModuleListMeta meta;
    };

    /**
     * @brief Maps a domain module to its public API representation.
     * @param module Domain module.
     * @return Module DTO with UTF-8 strings.
     */
    ModuleDto toDto(const entities::Module &module);

    /**
     * @brief Maps domain modules to the public collection response.
     * @param modules Domain modules.
     * @return Collection DTO and total count.
     */
    ModuleListDto toDto(const std::vector<entities::Module> &modules);

    /** @brief Serializes a module DTO to its public JSON fields. */
    void to_json(nlohmann::json &json, const ModuleDto &module); // NOLINT(readability-identifier-naming)

    /** @brief Parses a module DTO from its public JSON fields. */
    void from_json(const nlohmann::json &json, ModuleDto &module); // NOLINT(readability-identifier-naming)

    /** @brief Serializes collection metadata. */
    void to_json(nlohmann::json &json, const ModuleListMeta &meta); // NOLINT(readability-identifier-naming)

    /** @brief Parses collection metadata. */
    void from_json(const nlohmann::json &json, ModuleListMeta &meta); // NOLINT(readability-identifier-naming)

    /** @brief Serializes the module collection response. */
    void to_json(nlohmann::json &json, const ModuleListDto &modules); // NOLINT(readability-identifier-naming)

    /** @brief Parses the module collection response. */
    void from_json(const nlohmann::json &json, ModuleListDto &modules); // NOLINT(readability-identifier-naming)
} // namespace cppforge::api::dto
