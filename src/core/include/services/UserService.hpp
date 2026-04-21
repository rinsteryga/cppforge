#pragma once

#include "../repositories/IUserRepository.hpp"

#include <cstdint>
#include <memory>

namespace cppforge::services
{
    /**
     * @brief Service providing high-level operations on user profiles and statistics.
     *
     * UserService acts as a bridge between the data layer (IUserRepository) and the
     * application's business needs, such as retrieving solved task counts and
     * managing user metadata.
     */
    class UserService
    {
    public:
        /**
         * @brief Constructs a UserService with a user repository.
         * @param userRepository A pointer to an implementation of IUserRepository.
         */
        explicit UserService(repositories::IUserRepository &userRepository);

        /**
         * @brief Retrieves the total number of unique coding tasks solved by a user.
         * @param userId The ID of the user.
         * @return The count of unique successfully solved coding tasks.
         */
        int getSolvedTasksCount(uint64_t userId) const;

    private:
        repositories::IUserRepository &userRepository_;
    };
} // namespace cppforge::services
