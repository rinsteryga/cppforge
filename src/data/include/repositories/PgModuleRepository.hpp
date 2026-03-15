#pragma once

#include "../../core/include/repositories/IModuleRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge
{
    namespace repositories
    {
        class PgModuleRepository : public IModuleRepository
        {
        public:
            explicit PgModuleRepository(QSqlDatabase &database);

            std::vector<entities::Module> getAllModules() const override;
            std::optional<entities::Module> getModuleById(uint64_t id) const override;

        private:
            QSqlDatabase &database_;
        };
    } // namespace repositories
} // namespace cppforge
