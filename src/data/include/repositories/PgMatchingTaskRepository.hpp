#pragma once

#include "../../core/include/repositories/IMatchingTaskRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge
{
    namespace repositories
    {
        class PgMatchingTaskRepository : public IMatchingTaskRepository
        {
        public:
            explicit PgMatchingTaskRepository(QSqlDatabase &database);

            std::vector<entities::MatchingTask> getMatchingTasksByLessonId(uint64_t lessonId) const override;
            std::optional<entities::MatchingTask> getMatchingTaskById(uint64_t id) const override;

        private:
            QSqlDatabase &database_;
        };
    } // namespace repositories
} // namespace cppforge
