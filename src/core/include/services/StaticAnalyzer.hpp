#pragma once

#include "../entities/CodingTask.hpp"

#include <QString>

#include <optional>

namespace cppforge
{
    namespace services
    {
        class StaticAnalyzer
        {
        public:
            StaticAnalyzer() = default;
            ~StaticAnalyzer() = default;

            std::optional<QString> analyze(const entities::CodingTask &task, const QString &code) const;
        };
    } // namespace services
} // namespace cppforge
