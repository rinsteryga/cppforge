#pragma once

#include <QString>

#include <cstdint>

namespace cppforge::entities
{
    /**
     * @brief Represents a single selectable answer inside a Quiz.
     *
     * A QuizOption holds the text of a potential answer and a boolean
     * flag indicating whether it is the correct response.
     */
    class QuizOption
    {
    public:
        QuizOption() = default;

        /**
         * @brief Constructs a new QuizOption instance.
         *
         * @param optionId The unique identifier of this specific option.
         * @param optionText The display text of the answer option.
         * @param isCorrect A boolean determining if this option is the right answer.
         */
        QuizOption(uint64_t optionId, QString optionText, bool isCorrect);

        /**
         * @brief Retrieves the unique identifier of the option.
         * @return The 64-bit unsigned integer representing the option ID.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the display text for this option.
         * @return A constant reference to the option's text string.
         */
        const QString &getOptionText() const;

        /**
         * @brief Indicates if this option is flagged as correct.
         * @return True if the option represents the right answer; otherwise false.
         */
        bool isCorrect() const;

        /**
         * @brief Compares this option against another to enable strict ordering.
         *
         * Important for storing options inside an ordered std::set or analogous container,
         * sorting uniquely by their ID.
         *
         * @param other The comparable QuizOption entity.
         * @return True if this option's ID is strictly smaller than the other's.
         */
        bool operator<(const QuizOption &other) const;

    private:
        uint64_t id_;
        QString optionText_;
        bool isCorrect_;
    };
} // namespace cppforge::entities

#include <QMetaType>
Q_DECLARE_METATYPE(cppforge::entities::QuizOption)
