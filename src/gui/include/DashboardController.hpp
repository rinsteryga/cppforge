#pragma once

#include "../../core/include/services/CourseService.hpp"
#include "../../core/include/services/UserService.hpp"

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

namespace cppforge::gui
{
    /**
     * @class DashboardController
     * @brief Exposes course modules, daily goals, tips, user stats, roadmap, and duels to QML.
     */
    class DashboardController : public QObject
    {
        Q_OBJECT
        QML_ELEMENT

        Q_PROPERTY(QString username READ username NOTIFY userChanged)
        Q_PROPERTY(int userId READ userId NOTIFY userChanged)
        Q_PROPERTY(QString avatarPath READ avatarPath NOTIFY userChanged)
        Q_PROPERTY(QString tipOfTheDay READ tipOfTheDay NOTIFY tipChanged)
        Q_PROPERTY(int dailyGoalSolved READ dailyGoalSolved NOTIFY dailyGoalChanged)
        Q_PROPERTY(int dailyGoalTarget READ dailyGoalTarget CONSTANT)
        Q_PROPERTY(QVariantList modules READ modules NOTIFY modulesChanged)

        // Roadmap
        Q_PROPERTY(QVariantList roadmapNodes READ roadmapNodes NOTIFY roadmapChanged)
        Q_PROPERTY(int currentModuleId READ currentModuleId NOTIFY roadmapChanged)
        Q_PROPERTY(QString currentModuleTitle READ currentModuleTitle NOTIFY roadmapChanged)

        // Profile & Duel Stats
        Q_PROPERTY(int solvedTasksCount READ solvedTasksCount NOTIFY userChanged)
        Q_PROPERTY(int completedLessonsCount READ completedLessonsCount NOTIFY userChanged)
        Q_PROPERTY(int duelWins READ duelWins NOTIFY userChanged)
        Q_PROPERTY(int duelLosses READ duelLosses NOTIFY userChanged)
        Q_PROPERTY(int duelPoints READ duelPoints NOTIFY userChanged)
        Q_PROPERTY(double winrate READ winrate NOTIFY userChanged)
        Q_PROPERTY(int streak READ streak NOTIFY userChanged)
        Q_PROPERTY(QStringList availableAvatars READ availableAvatars CONSTANT)
        Q_PROPERTY(QVariantList achievements READ achievements NOTIFY userChanged)

    public:
        explicit DashboardController(QObject *parent = nullptr);
        ~DashboardController() override = default;

        void setUserService(cppforge::services::UserService *userService);
        void setCourseService(cppforge::services::CourseService *courseService);

        void setUser(const QString &username, int userId);

        QString username() const
        {
            return m_username;
        }
        int userId() const
        {
            return m_userId;
        }
        QString avatarPath() const
        {
            return m_avatarPath;
        }
        QString tipOfTheDay() const
        {
            return m_tipOfTheDay;
        }
        int dailyGoalSolved() const
        {
            return m_dailyGoalSolved;
        }
        int dailyGoalTarget() const
        {
            return 3;
        }
        QVariantList modules() const
        {
            return m_modules;
        }

        QVariantList roadmapNodes() const
        {
            return m_roadmapNodes;
        }
        int currentModuleId() const
        {
            return m_currentModuleId;
        }
        QString currentModuleTitle() const
        {
            return m_currentModuleTitle;
        }

        int solvedTasksCount() const
        {
            return m_solvedTasksCount;
        }
        int completedLessonsCount() const
        {
            return m_completedLessonsCount;
        }
        int duelWins() const
        {
            return m_duelWins;
        }
        int duelLosses() const
        {
            return m_duelLosses;
        }
        int duelPoints() const
        {
            return m_duelPoints;
        }
        double winrate() const
        {
            return m_winrate;
        }
        int streak() const
        {
            return m_streak;
        }
        QStringList availableAvatars() const;
        QVariantList achievements() const
        {
            return m_achievements;
        }

        Q_INVOKABLE void refresh();
        Q_INVOKABLE void openModule(int moduleId);
        Q_INVOKABLE void loadRoadmap(int moduleId);
        Q_INVOKABLE void updateAvatar(const QString &avatarPath);
        Q_INVOKABLE void logout();

    signals:
        void userChanged();
        void tipChanged();
        void dailyGoalChanged();
        void modulesChanged();
        void roadmapChanged();
        void openRoadmapRequested(int moduleId, const QString &moduleTitle);
        void logoutRequested();

    private:
        void updateModules();
        void updateDailyGoal();
        void updateTip();
        void updateUserStats();

        cppforge::services::UserService *m_userService = nullptr;
        cppforge::services::CourseService *m_courseService = nullptr;

        QString m_username;
        int m_userId = -1;
        QString m_avatarPath;
        QString m_tipOfTheDay;
        int m_dailyGoalSolved = 0;
        QVariantList m_modules;

        int m_currentModuleId = 1;
        QString m_currentModuleTitle;
        QVariantList m_roadmapNodes;

        int m_solvedTasksCount = 0;
        int m_completedLessonsCount = 0;
        int m_duelWins = 0;
        int m_duelLosses = 0;
        int m_duelPoints = 0;
        double m_winrate = 0.0;
        int m_streak = 0;
        QVariantList m_achievements;
    };
} // namespace cppforge::gui
