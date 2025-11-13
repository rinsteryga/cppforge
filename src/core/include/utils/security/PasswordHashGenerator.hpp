#pragma once

#include <QString>

namespace cppforge
{
    namespace utils
    {
        namespace security
        {
            class PasswordHashGenerator
            {
            public:
                static QString generate(const QString &password);
                static bool verify(const QString &password, const QString &hash);
            };
        } // namespace security
    } // namespace utils
} // namespace cppforge
