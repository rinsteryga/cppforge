#pragma once

#include "../entities/CodingTask.hpp"

#include <QString>

#include <optional>

namespace cppforge::services
{
    /**
     * @brief Performs lexical analysis on source code to check for prohibited or required constructs.
     *
     * StaticAnalyzer verifies that the submitted solution adheres to the constraints
     * defined in a CodingTask, such as blacklisted keywords or whitelisted patterns.
     */
    class StaticAnalyzer
    {
    public:
        /**
         * @brief Default constructor.
         */
        StaticAnalyzer() = default;

        /**
         * @brief Default destructor.
         */
        ~StaticAnalyzer() = default;

        /**
         * @brief Scans the provided code against task-specific constraints.
         *
         * @param task THE CodingTask entity containing whitelist/blacklist rules.
         * @param code The source code to be analyzed.
         * @return std::optional containing an error message if violations were found;
         *         std::nullopt if the code is compliant.
         */
        std::optional<QString> analyze(const entities::CodingTask &task, const QString &code) const;
    };
} // namespace cppforge::services
