#include "../../include/services/StaticAnalyzer.hpp"

#include "../../include/entities/CodingTask.hpp"

#include <QString>

#include <optional>

namespace cppforge::services
{
    std::optional<QString> StaticAnalyzer::analyze(const entities::CodingTask &task, const QString &code) const
    {
        if (task.getBlacklist().has_value())
        {
            for (const QString &forbidden : task.getBlacklist().value())
            {
                if (code.contains(forbidden))
                {
                    return QString("Analysis error: code contains a forbidden word: %1").arg(forbidden);
                }
            }
        }

        if (task.getWhitelist().has_value())
        {
            for (const QString &required : task.getWhitelist().value())
            {
                if (!code.contains(required))
                {
                    return QString("Analysis error: Missing required word: %1").arg(required);
                }
            }
        }

        return std::nullopt;
    }
} // namespace cppforge::services