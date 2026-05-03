#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace cppforge
{
    namespace entities
    {
        class Lesson;
    }

    namespace repositories
    {
        /**
         * @brief Defines the abstract data access contract for Lesson entities.
         *
         * This interface abstracts the persistence layer for acquiring learning lessons,
         * permitting modular swap-outs of the backend database.
         */
        class ILessonRepository
        {
        protected:
            ILessonRepository() = default;

        public:
            virtual ~ILessonRepository() = default;
            ILessonRepository(const ILessonRepository &) = delete;
            ILessonRepository &operator=(const ILessonRepository &) = delete;
            ILessonRepository(ILessonRepository &&) = delete;
            ILessonRepository &operator=(ILessonRepository &&) = delete;

            /**
             * @brief Retrieves an ordered collection of all lessons inside a specified module.
             *
             * @param moduleId The unique identifier of the container module.
             * @return A vector of Lesson entities associated with the given module.
             */
            virtual std::vector<entities::Lesson> getLessonsByModuleId(uint64_t moduleId) const = 0;

            /**
             * @brief Locates and returns a single lesson by its exact identifier.
             *
             * @param lessonId The unique identifier of the lesson.
             * @return An std::optional containing the Lesson if successful; otherwise std::nullopt.
             */
            virtual std::optional<entities::Lesson> getLessonById(uint64_t lessonId) const = 0;

            /**
             * @brief Gets the ID of the chronologically next lesson after the given one.
             * @param currentLessonId The unique identifier of the current lesson.
             * @return An std::optional containing the next lesson ID if available.
             */
            virtual std::optional<uint64_t> getNextLessonId(uint64_t currentLessonId) const = 0;

            /**
             * @brief Gets the ID of the chronologically previous lesson before the given one.
             * @param currentLessonId The unique identifier of the current lesson.
             * @return An std::optional containing the previous lesson ID if available.
             */
            virtual std::optional<uint64_t> getPrevLessonId(uint64_t currentLessonId) const = 0;
        };
    } // namespace repositories
} // namespace cppforge
