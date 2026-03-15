#pragma once

#include <QString>

#include <cstdint>

namespace cppforge::entities
{
    /**
     * @brief Represents a single correct pair in a matching exercise.
     *
     * A MatchingPair connects two related concepts (e.g., a term to its definition)
     * as part of a larger MatchingTask evaluation.
     */
    class MatchingPair
    {
    public:
        /**
         * @brief Constructs a new MatchingPair instance.
         *
         * @param pairId The unique identifier of this specific matching pair.
         * @param leftItem The first half of the logical pair (e.g., the concept).
         * @param rightItem The corresponding correct match (e.g., the definition).
         */
        MatchingPair(uint64_t pairId, QString leftItem, QString rightItem);

        /**
         * @brief Retrieves the unique identifier of this matching pair.
         * @return The 64-bit unsigned integer representing the ID.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the text for the left item of the pair.
         * @return A constant reference to the left side item's string.
         */
        const QString &getLeftItem() const;

        /**
         * @brief Retrieves the text for the right item of the pair.
         * @return A constant reference to the right side item's string.
         */
        const QString &getRightItem() const;

        /**
         * @brief Compares this matching pair against another to enable strict ordering.
         *
         * This defines a less-than operator based on the ID allowing the pair to be stored safely in sets.
         *
         * @param other The comparable MatchingPair entity.
         * @return True if this pair is strictly smaller than the other.
         */
        bool operator<(const MatchingPair &other) const;

    private:
        uint64_t id_;
        QString leftItem_;
        QString rightItem_;
    };
} // namespace cppforge::entities
