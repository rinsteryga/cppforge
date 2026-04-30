#pragma once

#include "../../../core/include/repositories/ILessonRepository.hpp"

#include <QtSql/QSqlDatabase>

namespace cppforge::repositories
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
        std::optional<entities::Lesson> getLessonById(uint64_t lessonId) const override;
        /**
         * @brief Gets the ID of the chronologically next lesson.
         * @param currentLessonId The ID of the starting lesson.
         * @return The next lesson ID if found, std::nullopt otherwise.
         */
        std::optional<uint64_t> getNextLessonId(uint64_t currentLessonId) const override;

        /**
         * @brief Gets the ID of the chronologically previous lesson.
         * @param currentLessonId The ID of the starting lesson.
         * @return The previous lesson ID if found, std::nullopt otherwise.
         */
        std::optional<uint64_t> getPrevLessonId(uint64_t currentLessonId) const override;

    private:
        QSqlDatabase &database_;
    };
} // namespace cppforge::repositories
