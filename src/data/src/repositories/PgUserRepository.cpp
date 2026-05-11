#include "../../include/repositories/PgUserRepository.hpp"

#include "../../../core/include/entities/User.hpp"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <chrono>

namespace
{
    std::chrono::system_clock::time_point qDateTimeToChrono(const QDateTime &dt)
    {
        if (!dt.isValid())
        {
            return std::chrono::system_clock::time_point();
        }
        return std::chrono::system_clock::from_time_t(dt.toSecsSinceEpoch());
    }

    QDateTime chronoToQDateTime(const std::chrono::system_clock::time_point &tp)
    {
        if (tp == std::chrono::system_clock::time_point())
        {
            return QDateTime();
        }
        return QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(tp));
    }

    cppforge::entities::ConditionType stringToConditionType(const QString &str)
    {
        if (str == "LEVELS_COMPLETED")
        {
            return cppforge::entities::ConditionType::LevelsCompleted;
        }
        if (str == "STREAK_DAYS")
        {
            return cppforge::entities::ConditionType::StreakDays;
        }
        return cppforge::entities::ConditionType::CustomEvent;
    }
} // namespace

namespace cppforge::repositories
{
    PgUserRepository::PgUserRepository(QSqlDatabase &database) : database_(database) {}

    std::optional<entities::User> PgUserRepository::findById(uint64_t id) const
    {
        if (!database_.isOpen() || id == 0)
        {
            return std::nullopt;
        }

        QSqlQuery query(database_);
        query.prepare(
            "SELECT id, username, email, password_hash, salt, avatar_path, bio, created_at, "
            "current_streak_days, last_level_solved_at, duel_points, duel_wins, duel_losses FROM users WHERE id = :id");
        query.bindValue(":id", QVariant::fromValue(id));

        if (!query.exec() || !query.next())
        {
            return std::nullopt;
        }

        entities::User user(query.value("id").toULongLong(), query.value("username").toString(),
                            query.value("email").toString(), query.value("password_hash").toString(),
                            query.value("salt").toString(), qDateTimeToChrono(query.value("created_at").toDateTime()));

        user.setAvatarPath(query.value("avatar_path").toString());
        user.setBio(query.value("bio").toString());
        user.setCurrentStreakDays(query.value("current_streak_days").toUInt());
        user.setLastLevelSolvedAt(qDateTimeToChrono(query.value("last_level_solved_at").toDateTime()));
        user.setDuelPoints(query.value("duel_points").toInt());
        user.setDuelWins(query.value("duel_wins").toInt());
        user.setDuelLosses(query.value("duel_losses").toInt());

        loadUserAchievements(user);
        loadUserCompletedLevels(user);

        return user;
    }

    std::optional<entities::User> PgUserRepository::findByEmail(const QString &email) const
    {
        if (!database_.isOpen() || email.isEmpty())
        {
            return std::nullopt;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT id, username, email, password_hash, salt, avatar_path, bio, created_at, "
                      "current_streak_days, last_level_solved_at, duel_points, duel_wins, duel_losses FROM users WHERE "
                      "email = :email");
        query.bindValue(":email", email);

        if (!query.exec() || !query.next())
        {
            return std::nullopt;
        }

        entities::User user(query.value("id").toULongLong(), query.value("username").toString(),
                            query.value("email").toString(), query.value("password_hash").toString(),
                            query.value("salt").toString(), qDateTimeToChrono(query.value("created_at").toDateTime()));

        user.setAvatarPath(query.value("avatar_path").toString());
        user.setBio(query.value("bio").toString());
        user.setCurrentStreakDays(query.value("current_streak_days").toUInt());
        user.setLastLevelSolvedAt(qDateTimeToChrono(query.value("last_level_solved_at").toDateTime()));
        user.setDuelPoints(query.value("duel_points").toInt());
        user.setDuelWins(query.value("duel_wins").toInt());
        user.setDuelLosses(query.value("duel_losses").toInt());

        loadUserAchievements(user);
        loadUserCompletedLevels(user);

        return user;
    }

    std::optional<entities::User> PgUserRepository::findByUsername(const QString &username) const
    {
        if (!database_.isOpen() || username.isEmpty())
        {
            return std::nullopt;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT id, username, email, password_hash, salt, avatar_path, bio, created_at, "
                      "current_streak_days, last_level_solved_at, duel_points, duel_wins, duel_losses FROM users WHERE "
                      "username = :username");
        query.bindValue(":username", username);

        if (!query.exec() || !query.next())
        {
            return std::nullopt;
        }

        entities::User user(query.value("id").toULongLong(), query.value("username").toString(),
                            query.value("email").toString(), query.value("password_hash").toString(),
                            query.value("salt").toString(), qDateTimeToChrono(query.value("created_at").toDateTime()));

        user.setAvatarPath(query.value("avatar_path").toString());
        user.setBio(query.value("bio").toString());
        user.setCurrentStreakDays(query.value("current_streak_days").toUInt());
        user.setLastLevelSolvedAt(qDateTimeToChrono(query.value("last_level_solved_at").toDateTime()));
        user.setDuelPoints(query.value("duel_points").toInt());
        user.setDuelWins(query.value("duel_wins").toInt());
        user.setDuelLosses(query.value("duel_losses").toInt());

        loadUserAchievements(user);
        loadUserCompletedLevels(user);

        return user;
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
            query.prepare("INSERT INTO users (username, email, password_hash, salt, avatar_path, bio, created_at, "
                          "current_streak_days, last_level_solved_at, duel_points, duel_wins, duel_losses) "
                          "VALUES (:username, :email, :password_hash, :salt, :avatar_path, :bio, :reg_date, "
                          ":streak, :solved_at, :duel_points, :duel_wins, :duel_losses) RETURNING id");
        }
        else
        {
            query.prepare(
                "UPDATE users SET username = :username, email = :email, password_hash = :password_hash, "
                "salt = :salt, avatar_path = :avatar_path, bio = :bio, "
                "current_streak_days = :streak, last_level_solved_at = :solved_at, "
                "duel_points = :duel_points, duel_wins = :duel_wins, duel_losses = :duel_losses WHERE id = :id");
            query.bindValue(":id", QVariant::fromValue(user.getId()));
        }

        query.bindValue(":username", user.getUsername());
        query.bindValue(":email", user.getEmail());
        query.bindValue(":password_hash", user.getPasswordHash());
        query.bindValue(":salt", user.getSalt());
        query.bindValue(":avatar_path", user.getAvatarPath());
        query.bindValue(":bio", user.getBio());
        query.bindValue(":reg_date", chronoToQDateTime(user.getCreatedAt()));
        query.bindValue(":streak", user.getCurrentStreakDays());
        query.bindValue(":solved_at", chronoToQDateTime(user.getLastLevelSolvedAt()));
        query.bindValue(":duel_points", user.getDuelPoints());
        query.bindValue(":duel_wins", user.getDuelWins());
        query.bindValue(":duel_losses", user.getDuelLosses());

        if (!query.exec())
        {
            qDebug() << "Failed to save user:" << query.lastError().text();
            return false;
        }

        saveUserAchievements(user);

        return true;
    }

    int PgUserRepository::getSolvedTasksCount(uint64_t userId) const
    {
        if (!database_.isOpen() || userId == 0)
        {
            return 0;
        }

        QSqlQuery query(database_);
        query.prepare(
            "SELECT COUNT(DISTINCT coding_task_id) FROM submissions WHERE user_id = :id AND is_success = true");
        query.bindValue(":id", QVariant::fromValue(userId));

        if (query.exec() && query.next())
        {
            return query.value(0).toInt();
        }

        return 0;
    }

    int PgUserRepository::getTodaySolvedTasksCount(uint64_t userId) const
    {
        if (!database_.isOpen() || userId == 0)
        {
            return 0;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT COUNT(DISTINCT coding_task_id) FROM submissions WHERE user_id = :id AND is_success = "
                      "true AND DATE(submitted_at) = CURRENT_DATE");
        query.bindValue(":id", QVariant::fromValue(userId));

        if (query.exec() && query.next())
        {
            return query.value(0).toInt();
        }

        return 0;
    }

    int PgUserRepository::getCompletedLessonsCount(uint64_t userId) const
    {
        if (!database_.isOpen() || userId == 0)
        {
            return 0;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT COUNT(*) FROM user_progress WHERE user_id = :id AND is_completed = true");
        query.bindValue(":id", QVariant::fromValue(userId));

        if (query.exec() && query.next())
        {
            return query.value(0).toInt();
        }

        return 0;
    }

    int PgUserRepository::getAchievementsCount(uint64_t userId) const
    {
        if (!database_.isOpen() || userId == 0)
        {
            return 0;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT COUNT(*) FROM user_achievements WHERE user_id = :id");
        query.bindValue(":id", QVariant::fromValue(userId));

        if (query.exec() && query.next())
        {
            return query.value(0).toInt();
        }

        return 0;
    }

    int PgUserRepository::getStreak(uint64_t userId) const
    {
        if (!database_.isOpen() || userId == 0)
        {
            return 0;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT current_streak_days FROM users WHERE id = :id");
        query.bindValue(":id", QVariant::fromValue(userId));

        if (query.exec() && query.next())
        {
            return query.value(0).toInt();
        }

        return 0;
    }

    int PgUserRepository::getTotalSubmissionsCount(uint64_t userId) const
    {
        if (!database_.isOpen() || userId == 0)
        {
            return 0;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT COUNT(*) FROM submissions WHERE user_id = :id");
        query.bindValue(":id", QVariant::fromValue(userId));

        if (query.exec() && query.next())
        {
            return query.value(0).toInt();
        }

        return 0;
    }

    std::vector<IUserRepository::Activity> PgUserRepository::getRecentActivity(uint64_t userId, int limit) const
    {
        std::vector<IUserRepository::Activity> activities;
        if (!database_.isOpen() || userId == 0)
        {
            return activities;
        }

        QSqlQuery query(database_);
        query.prepare(R"(
            (SELECT title, 'lesson' as type, updated_at as date 
             FROM user_progress 
             JOIN lessons ON user_progress.lesson_id = lessons.id 
             WHERE user_id = :id AND is_completed = true)
            UNION ALL
            (SELECT name as title, 'achievement' as type, earned_at as date 
             FROM user_achievements 
             JOIN achievements ON user_achievements.achievement_id = achievements.id 
             WHERE user_id = :id)
            ORDER BY date DESC LIMIT :limit
        )");
        query.bindValue(":id", QVariant::fromValue(userId));
        query.bindValue(":limit", limit);

        if (query.exec())
        {
            while (query.next())
            {
                activities.push_back({query.value("title").toString(), query.value("type").toString(),
                                      query.value("date").toDateTime().toString("dd.MM.yyyy")});
            }
        }

        return activities;
    }

    std::vector<uint64_t> PgUserRepository::getEarnedAchievementIds(uint64_t userId) const
    {
        std::vector<uint64_t> ids;
        if (!database_.isOpen() || userId == 0)
        {
            return ids;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT achievement_id FROM user_achievements WHERE user_id = :id");
        query.bindValue(":id", QVariant::fromValue(userId));

        if (query.exec())
        {
            while (query.next())
            {
                ids.push_back(query.value(0).toULongLong());
            }
        }

        return ids;
    }

    void PgUserRepository::loadUserAchievements(entities::User &user) const
    {
        QSqlQuery query(database_);
        query.prepare("SELECT achievement_id, name, description, icon_path, condition_type, condition_value, earned_at "
                      "FROM user_achievements "
                      "JOIN achievements ON user_achievements.achievement_id = achievements.id WHERE user_id = :id");
        query.bindValue(":id", QVariant::fromValue(user.getId()));

        if (query.exec())
        {
            while (query.next())
            {
                entities::Achievement achievement(
                    query.value("achievement_id").toLongLong(), query.value("name").toString(),
                    query.value("description").toString(), query.value("icon_path").toString(),
                    stringToConditionType(query.value("condition_type").toString()),
                    query.value("condition_value").toUInt(), qDateTimeToChrono(query.value("earned_at").toDateTime()));
                user.addAchievement(achievement);
            }
        }
    }

    void PgUserRepository::loadUserCompletedLevels(entities::User &user) const
    {
        QSqlQuery query(database_);
        query.prepare("SELECT lesson_id FROM user_progress WHERE user_id = :id AND is_completed = true");
        query.bindValue(":id", QVariant::fromValue(user.getId()));

        if (query.exec())
        {
            while (query.next())
            {
                user.addCompletedLevel(query.value(0).toULongLong());
            }
        }
    }

    void PgUserRepository::saveUserAchievements(const entities::User &user)
    {
        QSqlQuery deleteQuery(database_);
        deleteQuery.prepare("DELETE FROM user_achievements WHERE user_id = :id");
        deleteQuery.bindValue(":id", QVariant::fromValue(user.getId()));
        deleteQuery.exec();

        for (const auto &achievement : user.getAchievements())
        {
            QSqlQuery insertQuery(database_);
            insertQuery.prepare(
                "INSERT INTO user_achievements (user_id, achievement_id, earned_at) VALUES (:uid, :aid, :date)");
            insertQuery.bindValue(":uid", QVariant::fromValue(user.getId()));
            insertQuery.bindValue(":aid", QVariant::fromValue(achievement.getId()));
            insertQuery.bindValue(":date", chronoToQDateTime(achievement.getDateEarned()));
            insertQuery.exec();
        }
    }

    void PgUserRepository::updateStreak(uint64_t userId)
    {
        if (!database_.isOpen() || userId == 0)
        {
            return;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT current_streak_days, last_level_solved_at FROM users WHERE id = :id");
        query.bindValue(":id", QVariant::fromValue(userId));

        if (query.exec() && query.next())
        {
            uint32_t currentStreak = query.value("current_streak_days").toUInt();
            QDateTime lastSolvedAt = query.value("last_level_solved_at").toDateTime();
            QDateTime now = QDateTime::currentDateTime();

            if (!lastSolvedAt.isValid())
            {
                currentStreak = 1;
            }
            else
            {
                int daysSince = lastSolvedAt.daysTo(now);
                if (daysSince == 1)
                {
                    currentStreak++;
                }
                else if (daysSince > 1)
                {
                    currentStreak = 1;
                }
            }

            QSqlQuery updateQuery(database_);
            updateQuery.prepare(
                "UPDATE users SET current_streak_days = :streak, last_level_solved_at = :solved_at WHERE id = :id");
            updateQuery.bindValue(":streak", currentStreak);
            updateQuery.bindValue(":solved_at", now);
            updateQuery.bindValue(":id", QVariant::fromValue(userId));
            updateQuery.exec();
        }
    }

    void PgUserRepository::updateAvatar(uint64_t userId, const QString &avatarPath)
    {
        if (!database_.isOpen() || userId == 0)
        {
            return;
        }

        QSqlQuery query(database_);
        query.prepare("UPDATE users SET avatar_path = :path WHERE id = :id");
        query.bindValue(":path", avatarPath);
        query.bindValue(":id", QVariant::fromValue(userId));
        query.exec();
    }

    bool PgUserRepository::saveLessonProgress(uint64_t userId, uint64_t moduleId, uint64_t lessonId, bool isCompleted)
    {
        if (!database_.isOpen() || userId == 0)
            return false;

        QSqlQuery query(database_);
        query.prepare(R"(
                INSERT INTO user_progress (user_id, module_id, lesson_id, is_completed, updated_at) 
                VALUES (:uid, :mid, :lid, :status, CURRENT_TIMESTAMP) 
                ON CONFLICT (user_id, lesson_id) 
                DO UPDATE SET 
                    is_completed = EXCLUDED.is_completed, 
                    updated_at = CURRENT_TIMESTAMP
            )");

        query.bindValue(":uid", static_cast<qulonglong>(userId));
        query.bindValue(":mid", static_cast<qulonglong>(moduleId));
        query.bindValue(":lid", static_cast<qulonglong>(lessonId));
        query.bindValue(":status", isCompleted);

        return query.exec();
    }

    bool PgUserRepository::saveSubmission(uint64_t userId, uint64_t moduleId, uint64_t taskId, const QString &code,
                                          bool isSuccess)
    {
        if (!database_.isOpen() || userId == 0)
        {
            return false;
        }

        QSqlQuery query(database_);
        query.prepare(R"(
                INSERT INTO submissions (user_id, module_id, coding_task_id, source_code, is_success, submitted_at) 
                VALUES (:uid, :mid, :tid, :code, :success, CURRENT_TIMESTAMP)
            )");

        query.bindValue(":uid", static_cast<qulonglong>(userId));
        query.bindValue(":mid", static_cast<qulonglong>(moduleId));
        query.bindValue(":tid", static_cast<qulonglong>(taskId));
        query.bindValue(":code", code);
        query.bindValue(":success", isSuccess);

        return query.exec();
    }

    std::optional<QString> PgUserRepository::getLastSubmission(uint64_t userId, uint64_t taskId) const
    {
        if (!database_.isOpen() || userId == 0)
        {
            return std::nullopt;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT source_code FROM submissions WHERE user_id = :uid AND coding_task_id = :tid ORDER BY "
                      "submitted_at DESC LIMIT 1");
        query.bindValue(":uid", static_cast<qulonglong>(userId));
        query.bindValue(":tid", static_cast<qulonglong>(taskId));

        if (query.exec() && query.next())
        {
            return query.value(0).toString();
        }
        return std::nullopt;
    }

    bool PgUserRepository::isLessonCompleted(uint64_t userId, uint64_t lessonId) const
    {
        if (!database_.isOpen() || userId == 0)
            return false;

        QSqlQuery query(database_);
        query.prepare("SELECT is_completed FROM user_progress WHERE user_id = :uid AND lesson_id = :lid");
        query.bindValue(":uid", static_cast<qulonglong>(userId));
        query.bindValue(":lid", static_cast<qulonglong>(lessonId));

        if (query.exec() && query.next())
        {
            return query.value(0).toBool();
        }
        return false;
    }

    std::vector<uint64_t> PgUserRepository::getCompletedLessonIds(uint64_t userId, uint64_t moduleId) const
    {
        std::vector<uint64_t> ids;
        if (!database_.isOpen() || userId == 0)
            return ids;

        QSqlQuery query(database_);
        query.prepare(
            "SELECT lesson_id FROM user_progress WHERE user_id = :uid AND module_id = :mid AND is_completed = "
            "TRUE");
        query.bindValue(":uid", static_cast<qulonglong>(userId));
        query.bindValue(":mid", static_cast<qulonglong>(moduleId));

        if (query.exec())
        {
            while (query.next())
            {
                ids.push_back(query.value(0).toULongLong());
            }
        }
        return ids;
    }

    int PgUserRepository::getModuleProgress(uint64_t userId, uint64_t moduleId) const
    {
        if (!database_.isOpen() || userId == 0 || moduleId == 0)
            return 0;

        QSqlQuery query(database_);
        query.prepare(R"(
            SELECT 
                (SELECT COUNT(*) FROM lessons WHERE module_id = :mid) as total,
                (SELECT COUNT(*) FROM user_progress 
                 WHERE module_id = :mid AND user_id = :uid AND is_completed = true) as completed
        )");
        query.bindValue(":mid", static_cast<qulonglong>(moduleId));
        query.bindValue(":uid", static_cast<qulonglong>(userId));

        if (query.exec() && query.next())
        {
            int total = query.value("total").toInt();
            int completed = query.value("completed").toInt();
            if (total <= 0)
                return 0;
            return (completed * 100) / total;
        }
        return 0;
    }
} // namespace cppforge::repositories
