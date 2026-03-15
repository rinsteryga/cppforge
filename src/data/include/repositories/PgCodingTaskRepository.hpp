#pragma once

#include "../../core/include/repositories/ICodingTaskRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge
{
    namespace repositories
    {
        class PgCodingTaskRepository : public ICodingTaskRepository
        {
        public:
            explicit PgCodingTaskRepository(QSqlDatabase &database);

            std::vector<entities::CodingTask> getTasksByLessonId(uint64_t lessonId) const override;
            std::optional<entities::CodingTask> getTaskById(uint64_t id) const override;

        private:
            QSqlDatabase &database_;
        };
    } // namespace repositories
} // namespace cppforge
