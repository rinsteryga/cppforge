#pragma once

#include "../../../core/include/repositories/ILessonRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge
{
    namespace repositories
    {
        /**
         * @brief PostgreSQL-backed implementation of the Lesson repository.
         *
         * Provides access to lesson content and metadata stored in the database.
         */
        class PgLessonRepository : public ILessonRepository
        {
        public:
            /**
             * @brief Constructs a PgLessonRepository with a database connection.
             * @param database Reference to an open QSqlDatabase.
             */
            explicit PgLessonRepository(QSqlDatabase &database);

            /**
             * @brief Retrieves all lessons belonging to a specific module.
             * @param moduleId The ID of the parent module.
             * @return A vector of Lesson entities.
             */
            std::vector<entities::Lesson> getLessonsByModuleId(uint64_t moduleId) const override;

            /**
             * @brief Fetches a specific lesson by its unique ID.
             * @param id The lesson identifier.
             * @return Lesson if found, std::nullopt otherwise.
             */
            std::optional<entities::Lesson> getLessonById(uint64_t id) const override;

        private:
            QSqlDatabase &database_;
        };
    } // namespace repositories
} // namespace cppforge
