#include "../../include/repositories/PgAchievementRepository.hpp"

#include <QDateTime>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

namespace cppforge::repositories
{
    PgAchievementRepository::PgAchievementRepository(QSqlDatabase &database) : database_(database) {}

    std::vector<entities::Achievement> PgAchievementRepository::findAll() const
    {
        std::vector<entities::Achievement> achievements;

        if (!database_.isOpen())
        {
            return achievements;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT id, name, description, icon_path, condition_type, condition_value FROM achievements");

        if (query.exec())
        {
            auto parseConditionType = [](const QString &typeStr)
            {
                if (typeStr == "LEVELS_COMPLETED")
                {
                    return entities::ConditionType::LevelsCompleted;
                }
                if (typeStr == "STREAK_DAYS")
                {
                    return entities::ConditionType::StreakDays;
                }
                return entities::ConditionType::CustomEvent;
            };

            while (query.next())
            {
                entities::ConditionType cType = parseConditionType(query.value("condition_type").toString());
                uint32_t cValue = query.value("condition_value").toUInt();

                entities::Achievement achievement(query.value("id").toLongLong(), query.value("name").toString(),
                                                  query.value("description").toString(),
                                                  query.value("icon_path").toString(), cType, cValue,
                                                  std::chrono::system_clock::time_point{});
                achievements.push_back(std::move(achievement));
            }
        }

        return achievements;
    }
} // namespace cppforge::repositories
