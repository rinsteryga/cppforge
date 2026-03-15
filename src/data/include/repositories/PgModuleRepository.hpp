#pragma once

#include "../../../core/include/repositories/IModuleRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge::repositories
{
    /**
     * @brief PostgreSQL-backed implementation of the Module repository.
     *
     * Manages top-level course modules and their structural metadata.
     */
    class PgModuleRepository : public IModuleRepository
    {
    public:
        /**
         * @brief Constructs a PgModuleRepository with a database connection.
         * @param database Reference to an open QSqlDatabase.
         */
        explicit PgModuleRepository(QSqlDatabase &database);

        /**
         * @brief Retrieves all available modules.
         * @return A vector of all Module entities.
         */
        std::vector<entities::Module> getAllModules() const override;

        /**
         * @brief Fetches a specific module by its ID.
         * @param id The module identifier.
         * @return Module if found, std::nullopt otherwise.
         */
        std::optional<entities::Module> getModuleById(uint64_t id) const override;

    private:
        QSqlDatabase &database_;
    };
} // namespace cppforge::repositories
