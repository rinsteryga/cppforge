#pragma once

#include "../../../core/include/repositories/IAchievementRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge::repositories
{
    /**
     * @brief PostgreSQL-backed implementation of the Achievement repository.
     *
     * Handles retrieval of achievement dictionaries from the PostgreSQL database
     * using Qt's SQL module.
     */
    class PgAchievementRepository : public IAchievementRepository
    {
    public:
        /**
         * @brief Constructs a PgAchievementRepository with a database connection.
         * @param database Reference to an open QSqlDatabase.
         */
        explicit PgAchievementRepository(QSqlDatabase &database);

        /**
         * @brief Retrieves all registered achievements.
         * @return A vector of Achievement objects available in the system.
         */
        std::vector<entities::Achievement> findAll() const override;

    private:
        QSqlDatabase &database_;
    };
} // namespace cppforge::repositories
