#pragma once

#include <entities/Module.hpp>
#include <repositories/IModuleRepository.hpp>

#include <cstdint>
#include <functional>
#include <optional>

namespace cppforge::app
{
    /**
     * @brief Retrieves one learning module by its identifier.
     */
    class GetModuleById
    {
    public:
        /**
         * @brief Constructs the use-case with its repository dependency.
         * @param repository Module repository.
         */
        explicit GetModuleById(const repositories::IModuleRepository &repository);

        /**
         * @brief Executes the module lookup.
         * @param moduleId Identifier to look up.
         * @return The module when it exists, otherwise std::nullopt.
         */
        std::optional<entities::Module> execute(std::uint64_t moduleId) const;

    private:
        std::reference_wrapper<const repositories::IModuleRepository> m_repository;
    };
} // namespace cppforge::app
