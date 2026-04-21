#pragma once

#include "../../../core/include/repositories/ICodingTaskRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge::repositories
{
    /**
     * @brief PostgreSQL-backed implementation of the Coding Task repository.
     *
     * Manages retrieval of coding challenges and tasks from the database.
     */
    class PgCodingTaskRepository : public ICodingTaskRepository
    {
    public:
        /**
         * @brief Constructs a PgCodingTaskRepository with a database connection.
         * @param database Reference to an open QSqlDatabase.
         */
        explicit PgCodingTaskRepository(QSqlDatabase &database);

        /**
         * @brief Retrieves all coding tasks associated with a specific lesson.
         * @param lessonId The ID of the lesson.
         * @return A vector of CodingTask entities.
         */
        std::vector<entities::CodingTask> getTasksByLessonId(uint64_t lessonId) const override;

        /**
         * @brief Fetches a specific coding task by its unique ID.
         * @param id The task identifier.
         * @return CodingTask if found, std::nullopt otherwise.
         */
        std::optional<entities::CodingTask> getTaskById(uint64_t id) const override;

        /**
         * @brief Fetches a random duel coding task by topic.
         * @param topic The topic to filter duel tasks by.
         * @return CodingTask if found, std::nullopt otherwise.
         */
        std::optional<entities::CodingTask> getRandomDuelTaskByTopic(const QString& topic) const override;

    private:
        QSqlDatabase &database_;
    };
} // namespace cppforge::repositories
