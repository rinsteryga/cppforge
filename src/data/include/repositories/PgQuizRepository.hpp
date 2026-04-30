#pragma once

#include "../../../core/include/repositories/IQuizRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge::repositories
{
    /**
     * @brief PostgreSQL-backed implementation of the Quiz repository.
     *
     * Manages retrieval of quiz data and questions associated with lessons.
     */
    class PgQuizRepository : public IQuizRepository
    {
    public:
        /**
         * @brief Constructs a PgQuizRepository with a database connection.
         * @param database Reference to an open QSqlDatabase.
         */
        explicit PgQuizRepository(QSqlDatabase &database);

        /**
         * @brief Retrieves all quizzes belonging to a specific lesson.
         * @param lessonId The ID of the lesson.
         * @return A vector of Quiz entities.
         */
        std::vector<entities::Quiz> getQuizzesByLessonId(uint64_t lessonId) const override;

        /**
         * @brief Fetches a specific quiz by its unique ID.
         * @param id The quiz identifier.
         * @return Quiz if found, std::nullopt otherwise.
         */
        std::optional<entities::Quiz> getQuizById(uint64_t quizId) const override;

    private:
        QSqlDatabase &database_;
    };
} // namespace cppforge::repositories
