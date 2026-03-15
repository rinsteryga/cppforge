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
        /**
         * @brief Defines the abstract data access contract for Module entities.
         * 
         * Provides methods to discover available curriculum modules and drill down
         * into specific module metadata.
         */
        class IModuleRepository
        {
        protected:
            IModuleRepository() = default;
        public:
            virtual ~IModuleRepository() = default;
            IModuleRepository(const IModuleRepository&) = delete;
            IModuleRepository& operator=(const IModuleRepository&) = delete;
            IModuleRepository(IModuleRepository&&) = delete;
            IModuleRepository& operator=(IModuleRepository&&) = delete;

            /**
             * @brief Retrieves the complete list of modules available in the platform.
             * @return A vector containing all Module entities found in the data store.
             */
            virtual std::vector<entities::Module> getAllModules() const = 0;

            /**
             * @brief Fetches a specific module by its unique identifier.
             * 
             * @param moduleId The unique identifier of the module.
             * @return An std::optional containing the Module if found; otherwise std::nullopt.
             */
            virtual std::optional<entities::Module> getModuleById(uint64_t moduleId) const = 0;
        };
    } // namespace repositories
} // namespace cppforge
