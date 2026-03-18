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
                // Если код содержит запрещенную подстроку (например, "system")
                if (code.contains(forbidden))
                {
                    return QString("Ошибка безопасности: использование '%1' запрещено!").arg(forbidden);
                }
            }
        }

        // 2. Проверка белого списка (Whitelist)
        if (task.getWhitelist().has_value())
        {
            for (const QString &required : task.getWhitelist().value())
            {
                // Если обязательная конструкция отсутствует
                if (!code.contains(required))
                {
                    return QString("В коде отсутствует обязательный элемент: '%1'").arg(required);
                }
            }
        }

        return std::nullopt; // Анализ пройден успешно
    }
} // namespace cppforge::services