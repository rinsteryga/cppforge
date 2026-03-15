#pragma once

#include <QString>

namespace cppforge::utils::security
{
    /**
     * @brief Utility for securely hashing passwords and verifying them.
     * 
     * PasswordHashGenerator provides static methods to transform raw passwords
     * into secure hashes (e.g., using PBKDF2 or SHA-256 with salts) and to 
     * compare existing hashes against user input.
     */
    class PasswordHashGenerator
    {
    public:
        /**
         * @brief Transforms a raw password string into a secure cryptographic hash.
         * @param password The clear-text password to be handled.
         * @return A QString containing the resulting secure hash.
         */
        static QString generate(const QString &password);

        /**
         * @brief Validates a raw password against an existing secure hash.
         * @param password The raw password input to check.
         * @param hash The previously generated secure hash.
         * @return True if the password matches the hash; otherwise false.
         */
        static bool verify(const QString &password, const QString &hash);
    };
} // namespace cppforge::utils::security
