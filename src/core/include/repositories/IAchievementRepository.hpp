#pragma once

#include "../entities/Achievement.hpp"

#include <vector>

namespace cppforge::repositories
{
    /**
     * @brief Defines the abstract data access contract for Achievement entities.
     *
     * Provides methods for retrieving available platform achievements.
     */
    class IAchievementRepository
    {
    public:
        IAchievementRepository() = default;
        virtual ~IAchievementRepository() = default;

        IAchievementRepository(const IAchievementRepository &) = delete;
        IAchievementRepository &operator=(const IAchievementRepository &) = delete;
        IAchievementRepository(IAchievementRepository &&) = delete;
        IAchievementRepository &operator=(IAchievementRepository &&) = delete;

        /**
         * @brief Retrieves all registered achievements in the system.
         * @return A vector containing all available Achievements.
         */
        virtual std::vector<entities::Achievement> findAll() const = 0;
    };
} // namespace cppforge::repositories
