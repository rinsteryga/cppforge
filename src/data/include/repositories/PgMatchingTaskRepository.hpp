#pragma once

#include "../../../core/include/repositories/IMatchingTaskRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge
{
    namespace repositories
    {
        /**
         * @brief PostgreSQL-backed implementation of the Matching Task repository.
         *
         * mananges matching-style exercises and their data persistence.
         */
        class PgMatchingTaskRepository : public IMatchingTaskRepository
        {
        public:
            /**
             * @brief Constructs a PgMatchingTaskRepository with a database connection.
             * @param database Reference to an open QSqlDatabase.
             */
            explicit PgMatchingTaskRepository(QSqlDatabase &database);

            /**
             * @brief Retrieves matching tasks associated with a lesson.
             * @param lessonId The ID of the lesson.
             * @return A vector of MatchingTask entities.
             */
            std::vector<entities::MatchingTask> getMatchingTasksByLessonId(uint64_t lessonId) const override;

            /**
             * @brief Fetches a matching task by its unique ID.
             * @param id The task identifier.
             * @return MatchingTask if found, std::nullopt otherwise.
             */
            std::optional<entities::MatchingTask> getMatchingTaskById(uint64_t id) const override;

        private:
            QSqlDatabase &database_;
        };
    } // namespace repositories
} // namespace cppforge
