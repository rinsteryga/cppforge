#pragma once

#include <cstdint>
#include <optional>
#include <QString>

namespace cppforge
{
    namespace entities
    {
        class User;
    }

    namespace repositories
    {

        class IUserRepository
        {
        public:
            virtual ~IUserRepository() = default;

            virtual std::optional<entities::User> findByEmail(const QString& email) const = 0;
            virtual void save(entities::User user) = 0;
        };
    } // namespace repositories
} // namespace cppforge
