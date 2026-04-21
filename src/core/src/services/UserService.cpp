#include "../../include/services/UserService.hpp"

namespace cppforge::services
{
    UserService::UserService(repositories::IUserRepository &userRepository) : userRepository_(userRepository) {}

    int UserService::getSolvedTasksCount(uint64_t userId) const
    {
        return userRepository_.getSolvedTasksCount(userId);
    }
} // namespace cppforge::services
