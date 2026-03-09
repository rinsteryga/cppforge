#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace cppforge
{
    namespace entities
    {
        class Module;
    }

    namespace repositories
    {
        class IModuleRepository
        {
        public:
            virtual ~IModuleRepository() = default;

            virtual std::vector<entities::Module> getAllModules() const = 0;
            virtual std::optional<entities::Module> getModuleById(uint64_t id) const = 0;
        };
    } // namespace repositories
} // namespace cppforge
