#pragma once

#include <QString>

#include <cstdint>
#include <set>

namespace cppforge::entities
{
    class QuizOption;

    /**
     * @brief Represents a multiple-choice question within a lesson.
     * 
     * A Quiz encapsulates a single query (with an optional title) and pairs it with
     * a collection of possible answers, out of which only one is correct.
     */
    class Quiz
    {
    public:
        /**
         * @brief Constructs a new Quiz instance.
         * 
         * @param quizId The unique identifier of the quiz.
         * @param lessonId The ID of the parent lesson managing this quiz.
         * @param title The concise display title or prefix of the quiz.
         * @param question The main interrogative text presented to the user.
         * @param options A complete set of distinct QuizOption items.
         */
        Quiz(uint64_t quizId, uint64_t lessonId, QString title, QString question, const std::set<QuizOption> &options);

        /**
         * @brief Retrieves the quiz's unique identifier.
         * @return The 64-bit unsigned integer representing the quiz ID.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the parent lesson's identifier.
         * @return The 64-bit unsigned int representing the lesson ID.
         */
        uint64_t getLessonId() const;

        /**
         * @brief Retrieves the display title of the quiz.
         * @return A constant reference to the QString containing the title.
         */
        const QString &getTitle() const;

        /**
         * @brief Retrieves the main question text of the quiz.
         * @return A constant reference to the QString containing the question.
         */
        const QString &getQuestion() const;

        /**
         * @brief Retrieves the set of possible answer options formulated for this quiz.
         * @return A constant reference to the set of QuizOption objects.
         */
        const std::set<QuizOption> &getOptions() const;

    private:
        uint64_t id_;
        uint64_t lessonId_;
        QString title_;
        QString question_;
        std::set<QuizOption> options_;
    };
} // namespace cppforge::entities
