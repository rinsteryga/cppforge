#pragma once

#include "../entities/Quiz.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace cppforge::repositories
{
    /**
     * @brief Defines the abstract data access contract for Quiz entities.
     * 
     * This interface enables fetching multiple-choice assessment tasks 
     * from the persistence layer.
     */
    class IQuizRepository
    {
    protected:
        IQuizRepository() = default;
    public:
        virtual ~IQuizRepository() = default;
        IQuizRepository(const IQuizRepository&) = delete;
        IQuizRepository& operator=(const IQuizRepository&) = delete;
        IQuizRepository(IQuizRepository&&) = delete;
        IQuizRepository& operator=(IQuizRepository&&) = delete;

        /**
         * @brief Retrieves a list of all quizzes registered for a specific lesson.
         * 
         * @param lessonId The unique identifier of the lesson.
         * @return A vector of Quiz entities for the given lesson.
         */
        virtual std::vector<entities::Quiz> getQuizzesByLessonId(uint64_t lessonId) const = 0;

        /**
         * @brief Fetches a specific quiz by its unique identifier.
         * 
         * @param quizId The unique identifier of the quiz.
         * @return An std::optional containing the Quiz if found; otherwise std::nullopt.
         */
        virtual std::optional<entities::Quiz> getQuizById(uint64_t quizId) const = 0;
    };
} // namespace cppforge::repositories
