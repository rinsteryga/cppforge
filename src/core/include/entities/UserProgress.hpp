#pragma once

#include <QString>

#include <cstdint>

namespace cppforge::entities
{
    class ExecutionResult;

    /**
     * @brief Serves as a mapping entity linking users to their modular progression.
     * 
     * UserProgress stores a transactional snapshot of whether a specific user 
     * has successfully concluded a discrete lesson within an explicit module block.
     */
    class UserProgress
    {
    public:
        /**
         * @brief Constructs a new UserProgress instance.
         * 
         * @param userId The ID of the student.
         * @param moduleId The overarching module umbrella enclosing the segment.
         * @param lessonId The explicit lesson being tracked.
         * @param isCompleted Status marking if the lesson meets full curriculum thresholds.
         */
        UserProgress(uint64_t userId, uint64_t moduleId, uint64_t lessonId, bool isCompleted);

        /**
         * @brief Retrieves the referenced user's identifier.
         * @return The 64-bit unsigned integer representing the user ID.
         */
        uint64_t getUserId() const;

        /**
         * @brief Retrieves the parent module's identifier.
         * @return The 64-bit unsigned integer representing the module ID.
         */
        uint64_t getModuleId() const;

        /**
         * @brief Retrieves the designated lesson's identifier.
         * @return The 64-bit unsigned int representing the lesson ID.
         */
        uint64_t getLessonId() const;

        /**
         * @brief Reports if the lesson tracked was thoroughly conquered.
         * @return True if the progress reflects a finalized state; otherwise false.
         */
        bool isCompleted() const;

        /**
         * @brief Hard-sets the boolean completion flag for the given progress unit.
         * @param completed The targeted boolean terminal state to override with.
         */
        void setCompleted(bool completed);

    private:
        uint64_t userId_;
        uint64_t moduleId_;
        uint64_t lessonId_;
        bool isCompleted_;
    };
} // namespace cppforge::entities
