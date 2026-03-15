#pragma once

#include "../../core/include/repositories/IQuizRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge
{
    namespace repositories
    {
        class PgQuizRepository : public IQuizRepository
        {
        public:
            explicit PgQuizRepository(QSqlDatabase &database);

            std::vector<entities::Quiz> getQuizzesByLessonId(uint64_t lessonId) const override;
            std::optional<entities::Quiz> getQuizById(uint64_t id) const override;

        private:
            QSqlDatabase &database_;
        };
    } // namespace repositories
} // namespace cppforge
