#include "../../include/repositories/PgUserRepository.hpp"

#include "../../../core/include/entities/Achievement.hpp"
#include "../../../core/include/entities/User.hpp"

#include <QDateTime>
#include <QDebug>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

namespace cppforge
{
    namespace repositories
    {
        PgUserRepository::PgUserRepository(QSqlDatabase &database) : database_(database) {}

        std::optional<entities::User> PgUserRepository::findByEmail(const QString &email) const
        {
            if (!database_.isOpen())
            {
                return std::nullopt;
            }

            QSqlQuery query(database_);
            query.prepare("SELECT id, username, email, password_hash, salt, avatar_path, bio, current_streak_days, "
                          "last_level_solved_at, created_at FROM users WHERE "
                          "email = :email");
            query.bindValue(":email", email);

            if (query.exec() && query.next())
            {
                uint64_t userId = query.value("id").toULongLong();
                QString username = query.value("username").toString();
                QString userEmail = query.value("email").toString();
                QString passwordHash = query.value("password_hash").toString();
                QString salt = query.value("salt").toString();
                QString avatarPath = query.value("avatar_path").toString();
                QString bio = query.value("bio").toString();

                auto createdAt =
                    std::chrono::system_clock::from_time_t(query.value("created_at").toDateTime().toSecsSinceEpoch());

                entities::User user(userId, username, userEmail, passwordHash, salt, createdAt);
                user.setAvatarPath(avatarPath);
                user.setBio(bio);
                user.setCurrentStreakDays(query.value("current_streak_days").toUInt());

                QVariant lastSolvedVar = query.value("last_level_solved_at");
                if (!lastSolvedVar.isNull() && lastSolvedVar.isValid())
                {
                    auto lastSolvedAt =
                        std::chrono::system_clock::from_time_t(lastSolvedVar.toDateTime().toSecsSinceEpoch());
                    user.setLastLevelSolvedAt(lastSolvedAt);
                }

                loadUserAchievements(user);
                loadUserCompletedLevels(user);

                return user;
            }

            return std::nullopt;
        }

        std::optional<entities::User> PgUserRepository::findByUsername(const QString &username) const
        {
            if (!database_.isOpen())
            {
                return std::nullopt;
            }

            QSqlQuery query(database_);
            query.prepare("SELECT id, username, email, password_hash, salt, avatar_path, bio, current_streak_days, "
                          "last_level_solved_at, created_at FROM users WHERE "
                          "username = :username");
            query.bindValue(":username", username);

            if (query.exec() && query.next())
            {
                uint64_t userId = query.value("id").toULongLong();
                QString fetchedUsername = query.value("username").toString();
                QString userEmail = query.value("email").toString();
                QString passwordHash = query.value("password_hash").toString();
                QString salt = query.value("salt").toString();
                QString avatarPath = query.value("avatar_path").toString();
                QString bio = query.value("bio").toString();

                auto createdAt =
                    std::chrono::system_clock::from_time_t(query.value("created_at").toDateTime().toSecsSinceEpoch());

                entities::User user(userId, fetchedUsername, userEmail, passwordHash, salt, createdAt);
                user.setAvatarPath(avatarPath);
                user.setBio(bio);
                user.setCurrentStreakDays(query.value("current_streak_days").toUInt());

                QVariant lastSolvedVar = query.value("last_level_solved_at");
                if (!lastSolvedVar.isNull() && lastSolvedVar.isValid())
                {
                    auto lastSolvedAt =
                        std::chrono::system_clock::from_time_t(lastSolvedVar.toDateTime().toSecsSinceEpoch());
                    user.setLastLevelSolvedAt(lastSolvedAt);
                }

                loadUserAchievements(user);
                loadUserCompletedLevels(user);

                return user;
            }

            return std::nullopt;
        }

        bool PgUserRepository::save(const entities::User &user)
        {
            if (!database_.isOpen())
            {
                return false;
            }

            QSqlQuery query(database_);
            if (user.getId() == 0)
            {
                query.prepare("INSERT INTO users (username, email, password_hash, salt, avatar_path, bio, "
                              "current_streak_days, last_level_solved_at) VALUES (:username, "
                              ":email, :password_hash, :avatar_path, :bio, :current_streak_days, "
                              ":last_level_solved_at) RETURNING id");
                query.bindValue(":username", user.getUsername());
                query.bindValue(":email", user.getEmail());
                query.bindValue(":password_hash", user.getPasswordHash());
                query.bindValue(":salt", user.getSalt());
                query.bindValue(":avatar_path", user.getAvatarPath());
                query.bindValue(":bio", user.getBio());
                query.bindValue(":current_streak_days", user.getCurrentStreakDays());

                auto lastSolvedEpoch = std::chrono::system_clock::to_time_t(user.getLastLevelSolvedAt());
                if (lastSolvedEpoch > 0)
                {
                    query.bindValue(":last_level_solved_at", QDateTime::fromSecsSinceEpoch(lastSolvedEpoch));
                }
                else
                {
                    query.bindValue(":last_level_solved_at", QVariant(QVariant::DateTime));
                }

                if (!query.exec())
                {
                    qDebug() << "INSERT failed inside PgUserRepository:" << query.lastError().text();
                    return false;
                }

                uint64_t newId = query.value("id").toULongLong();
                return true;
            }

            query.prepare("UPDATE users SET username = :username, email = :email, password_hash = :password_hash, "
                          "avatar_path = :avatar_path, bio = :bio, current_streak_days = :current_streak_days, "
                          "last_level_solved_at = :last_level_solved_at WHERE id = :id");
            query.bindValue(":username", user.getUsername());
            query.bindValue(":email", user.getEmail());
            query.bindValue(":password_hash", user.getPasswordHash());
            query.bindValue(":avatar_path", user.getAvatarPath());
            query.bindValue(":bio", user.getBio());
            query.bindValue(":current_streak_days", user.getCurrentStreakDays());

            auto lastSolvedEpoch = std::chrono::system_clock::to_time_t(user.getLastLevelSolvedAt());
            if (lastSolvedEpoch > 0)
            {
                query.bindValue(":last_level_solved_at", QDateTime::fromSecsSinceEpoch(lastSolvedEpoch));
            }
            else
            {
                query.bindValue(":last_level_solved_at", QVariant(QVariant::DateTime));
            }

            query.bindValue(":id", QVariant::fromValue(user.getId()));

            if (!query.exec())
            {
                qDebug() << "UPDATE failed inside PgUserRepository:" << query.lastError().text();
                return false;
            }

            saveUserAchievements(user);
            return true;
        }

        void PgUserRepository::loadUserAchievements(entities::User &user) const
        {
            if (!database_.isOpen() || user.getId() == 0)
            {
                return;
            }

            QSqlQuery query(database_);
            query.prepare(
                "SELECT a.id, a.name, a.description, a.icon_path, a.condition_type, a.condition_value, ua.earned_at "
                "FROM achievements a "
                "JOIN user_achievements ua ON a.id = ua.achievement_id "
                "WHERE ua.user_id = :user_id");
            query.bindValue(":user_id", QVariant::fromValue(user.getId()));

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
                    auto earnedAt = std::chrono::system_clock::from_time_t(
                        query.value("earned_at").toDateTime().toSecsSinceEpoch());

                    entities::ConditionType cType = parseConditionType(query.value("condition_type").toString());
                    uint32_t cValue = query.value("condition_value").toUInt();

                    entities::Achievement achievement(query.value("id").toLongLong(), query.value("name").toString(),
                                                      query.value("description").toString(),
                                                      query.value("icon_path").toString(), cType, cValue, earnedAt);
                    user.addAchievement(achievement);
                }
            }
        }

        void PgUserRepository::loadUserCompletedLevels(entities::User &user) const
        {
            if (!database_.isOpen() || user.getId() == 0)
            {
                return;
            }

            QSqlQuery query(database_);
            query.prepare("SELECT lesson_id FROM user_progress WHERE user_id = :user_id AND is_completed = TRUE");
            query.bindValue(":user_id", QVariant::fromValue(user.getId()));

            if (query.exec())
            {
                while (query.next())
                {
                    user.addCompletedLevel(query.value("lesson_id").toULongLong());
                }
            }
        }

        void PgUserRepository::saveUserAchievements(const entities::User &user)
        {
            if (!database_.isOpen() || user.getId() == 0)
            {
                return;
            }

            QSqlQuery query(database_);
            query.prepare("DELETE FROM user_achievements WHERE user_id = :user_id");
            query.bindValue(":user_id", QVariant::fromValue(user.getId()));
            query.exec();

            for (const auto &achievement : user.getAchievements())
            {
                QSqlQuery insertQuery(database_);
                insertQuery.prepare("INSERT INTO user_achievements (user_id, achievement_id, earned_at) VALUES "
                                    "(:user_id, :achievement_id, :earned_at)");
                insertQuery.bindValue(":user_id", QVariant::fromValue(user.getId()));
                insertQuery.bindValue(":achievement_id", QVariant::fromValue(achievement.getId()));
                insertQuery.bindValue(":earned_at", QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(
                                                        achievement.getDateEarned())));
                insertQuery.exec();
            }
        }
    } // namespace repositories
} // namespace cppforge
