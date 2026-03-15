#pragma once

#include <QString>

#include <cstdint>

namespace cppforge::entities
{
    class Module;

    /**
     * @brief Represents a single educational lesson within a module.
     * 
     * A Lesson is a fundamental teaching unit in the platform. It encapsulates 
     * educational content (like theoretical text or markdown) and is logically 
     * tied to a broader containing Module.
     */
    class Lesson
    {
    public:
        /**
         * @brief Constructs a new Lesson instance.
         * 
         * @param lessonId The unique identifier of the lesson.
         * @param moduleId The identifier of the parent module this lesson belongs to.
         * @param title The concise display title of the lesson.
         * @param content The main educational content or theory payload of the lesson.
         */
        Lesson(uint64_t lessonId, uint64_t moduleId, QString title, QString content);

        /**
         * @brief Retrieves the lesson's unique identifier.
         * @return The ID as a 64-bit unsigned integer.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the parent module's identifier.
         * @return The associated module ID as a 64-bit unsigned integer.
         */
        uint64_t getModuleId() const;

        /**
         * @brief Retrieves the display title of the lesson.
         * @return A constant reference to the QString containing the lesson's title.
         */
        const QString &getTitle() const;

        /**
         * @brief Retrieves the primary educational content payload.
         * @return A constant reference to the QString containing the content.
         */
        const QString &getContent() const;

    private:
        uint64_t id_;
        uint64_t moduleId_;
        QString title_;
        QString content_;
    };
} // namespace cppforge::entities
