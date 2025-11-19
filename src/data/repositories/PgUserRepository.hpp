#pragma once

#include "../../core/include/repositories/IUserRepository.hpp"

#include <optional>
#include <QString>
#include <QtSql/QSqlDatabase>

namespace cppforge
{
    namespace repositories
    {
        class PgUserRepository : public IUserRepository
        {
        public:
            explicit PgUserRepository(QSqlDatabase &database);

            std::optional<entities::User> findByEmail(const QString& email) const override;
            void save(entities::User user) override;

        private:
            QSqlDatabase& database_;
        };
    } // namespace repositories
} // namespace cppforge
