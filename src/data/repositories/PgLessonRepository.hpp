#pragma once

#include "../../core/include/repositories/ILessonRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge
{
    namespace repositories
    {
        class PgLessonRepository : public ILessonRepository
        {
        public:
            explicit PgLessonRepository(QSqlDatabase &database);

            std::vector<entities::Lesson> getLessonsByModuleId(uint64_t moduleId) const override;
            std::optional<entities::Lesson> getLessonById(uint64_t id) const override;

        private:
            QSqlDatabase &database_;
        };
    } // namespace repositories
} // namespace cppforge
