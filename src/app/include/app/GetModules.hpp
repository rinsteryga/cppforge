#pragma once

#include <entities/Module.hpp>
#include <repositories/IModuleRepository.hpp>

#include <functional>
#include <vector>

namespace cppforge::app
{
    /**
     * @brief Retrieves all learning modules through the domain repository interface.
     */
    class GetModules
    {
    public:
        /**
         * @brief Constructs the use-case with its repository dependency.
         * @param repository Module repository.
         */
        explicit GetModules(const repositories::IModuleRepository &repository);

        /**
         * @brief Executes the module-list query.
         * @return All available modules.
         * @throws std::exception Propagates repository failures to the caller.
         */
        std::vector<entities::Module> execute() const;

    private:
        std::reference_wrapper<const repositories::IModuleRepository> m_repository;
    };
} // namespace cppforge::app
