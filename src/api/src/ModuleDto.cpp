#include "api/dto/ModuleDto.hpp"

#include <entities/Module.hpp>
#include <nlohmann/json.hpp>

#include <QString>

namespace cppforge::api::dto
{
    namespace
    {
        std::string toUtf8(const QString &value)
        {
            const auto bytes = value.toUtf8();
            return {bytes.constData(), static_cast<std::size_t>(bytes.size())};
        }
    } // namespace

    ModuleDto toDto(const entities::Module &module)
    {
        return {.id = module.getId(), .name = toUtf8(module.getName()), .description = toUtf8(module.getDescription())};
    }

    ModuleListDto toDto(const std::vector<entities::Module> &modules)
    {
        ModuleListDto result;
        result.items.reserve(modules.size());
        for (const auto &module : modules)
        {
            result.items.push_back(toDto(module));
        }
        result.meta.total = result.items.size();
        return result;
    }

    void to_json(nlohmann::json &json, const ModuleDto &module)
    {
        json = {{"id", module.id}, {"name", module.name}, {"description", module.description}};
    }

    void from_json(const nlohmann::json &json, ModuleDto &module)
    {
        json.at("id").get_to(module.id);
        json.at("name").get_to(module.name);
        json.at("description").get_to(module.description);
    }

    void to_json(nlohmann::json &json, const ModuleListMeta &meta)
    {
        json = {{"total", meta.total}};
    }

    void from_json(const nlohmann::json &json, ModuleListMeta &meta)
    {
        json.at("total").get_to(meta.total);
    }

    void to_json(nlohmann::json &json, const ModuleListDto &modules)
    {
        json = {{"items", modules.items}, {"meta", modules.meta}};
    }

    void from_json(const nlohmann::json &json, ModuleListDto &modules)
    {
        json.at("items").get_to(modules.items);
        json.at("meta").get_to(modules.meta);
    }
} // namespace cppforge::api::dto
