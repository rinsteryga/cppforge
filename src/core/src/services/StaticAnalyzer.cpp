#include "../../include/services/StaticAnalyzer.hpp"

#include "../../include/entities/CodingTask.hpp"

#include <QString>

#include <optional>
#include <set>

namespace cppforge::services
{
    std::optional<QString> StaticAnalyzer::analyze(const entities::CodingTask &task, const QString &code) const
    {
        // 1. Проверка черного списка (Blacklist)
        if (task.getBlacklist().has_value())
        {
            for (const QString &forbidden : task.getBlacklist().value())
            {
                if (code.contains(forbidden))
                {
                    // Тест ожидает наличие фразы "forbidden word" в результате
                    return QString("Analysis error: code contains a forbidden word: %1").arg(forbidden);
                }
            }
        }

        // 2. Проверка белого списка (Whitelist)
        if (task.getWhitelist().has_value())
        {
            for (const QString &required : task.getWhitelist().value())
            {
                if (!code.contains(required))
                {
                    // Тест ожидает наличие фразы "Missing required word" в результате
                    return QString("Analysis error: Missing required word: %1").arg(required);
                }
            }
        }

        return std::nullopt; // Анализ пройден успешно
    }
} // namespace cppforge::services