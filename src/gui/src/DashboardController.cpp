#include "DashboardController.hpp"

#include <QSettings>

namespace cppforge::gui
{
    DashboardController::DashboardController(QObject *parent) : QObject(parent) {}

    void DashboardController::setUserService(cppforge::services::UserService *userService)
    {
        m_userService = userService;
    }

    void DashboardController::setCourseService(cppforge::services::CourseService *courseService)
    {
        m_courseService = courseService;
    }

    void DashboardController::setUser(const QString &username, int userId)
    {
        m_username = username;
        m_userId = userId;

        updateUserStats();
        emit userChanged();
        refresh();
    }

    void DashboardController::refresh()
    {
        updateTip();
        updateDailyGoal();
        updateModules();
        updateUserStats();
    }

    void DashboardController::updateUserStats()
    {
        if (!m_userService || m_userId <= 0)
        {
            return;
        }

        auto userOpt = m_userService->findById(static_cast<uint64_t>(m_userId));
        if (!userOpt)
        {
            return;
        }

        m_username = userOpt->getUsername();
        m_avatarPath = userOpt->getAvatarPath();
        if (m_avatarPath.isEmpty())
        {
            m_avatarPath = "qrc:/images/default_avatar1.png";
        }
        else if (m_avatarPath.startsWith(":/"))
        {
            m_avatarPath = "qrc" + m_avatarPath;
        }

        m_duelWins = userOpt->getDuelWins();
        m_duelLosses = userOpt->getDuelLosses();
        m_duelPoints = userOpt->getDuelPoints();
        int totalDuels = m_duelWins + m_duelLosses;
        m_winrate = (totalDuels > 0) ? (static_cast<double>(m_duelWins) / totalDuels * 100.0) : 0.0;
        m_streak = static_cast<int>(userOpt->getCurrentStreakDays());
        m_completedLessonsCount = static_cast<int>(userOpt->getCompletedLevelsIds().size());
        m_solvedTasksCount = m_userService->getTodaySolvedTasksCount(static_cast<uint64_t>(m_userId));

        // Achievements
        m_achievements.clear();
        const auto &achSet = userOpt->getAchievements();
        for (const auto &ach : achSet)
        {
            QVariantMap achMap;
            achMap["id"] = static_cast<int>(ach.getId());
            achMap["name"] = ach.getName();
            achMap["description"] = ach.getDescription();
            achMap["iconPath"] = ach.getIconPath();
            achMap["isUnlocked"] = true;
            m_achievements.append(achMap);
        }

        // Default showcase achievements if list is empty
        if (m_achievements.isEmpty())
        {
            static const struct
            {
                QString name;
                QString desc;
                QString icon;
                bool unlocked;
            } sampleAchs[] = {{"First Steps", "Complete your first lesson in CppForge", "🏆", true},
                              {"Duel Master", "Win your first multiplayer coding duel", "⚔️", false},
                              {"Code Streaker", "Maintain a 3-day study streak", "🔥", m_streak >= 3},
                              {"Template Wizard", "Solve all tasks in Templates module", "🧙", false},
                              {"Speed Demon", "Solve a task in under 60 seconds", "⚡", false}};

            for (int i = 0; i < 5; ++i)
            {
                QVariantMap achMap;
                achMap["id"] = i + 1;
                achMap["name"] = sampleAchs[i].name;
                achMap["description"] = sampleAchs[i].desc;
                achMap["iconPath"] = sampleAchs[i].icon;
                achMap["isUnlocked"] = sampleAchs[i].unlocked;
                m_achievements.append(achMap);
            }
        }
    }

    void DashboardController::updateTip()
    {
        if (m_courseService)
        {
            m_tipOfTheDay = m_courseService->getRandomTip();
            if (m_tipOfTheDay.isEmpty())
            {
                m_tipOfTheDay = "Use std::unique_ptr by default for single-owner dynamic resources in modern C++.";
            }
            emit tipChanged();
        }
    }

    void DashboardController::updateDailyGoal()
    {
        if (m_userService && m_userId > 0)
        {
            m_dailyGoalSolved = m_userService->getTodaySolvedTasksCount(static_cast<uint64_t>(m_userId));
            emit dailyGoalChanged();
        }
    }

    void DashboardController::updateModules()
    {
        m_modules.clear();

        std::vector<int> progresses;
        if (m_courseService && m_userId > 0)
        {
            progresses = m_courseService->getAllModulesProgress(static_cast<uint64_t>(m_userId));
        }

        static const QStringList moduleTitles = {"Basics of C++",
                                                 "Variables & Types",
                                                 "Operators & Control Flow",
                                                 "Functions & Pointers",
                                                 "References & Constness",
                                                 "OOP & Encapsulation",
                                                 "Inheritance & Polymorphism",
                                                 "Templates & Metaprogramming",
                                                 "STL Containers",
                                                 "STL Algorithms",
                                                 "Smart Pointers & RAII",
                                                 "Move Semantics & Rvalues",
                                                 "Multithreading & Concurrency",
                                                 "Modern C++ Standards (C++20/23)"};

        int totalModules = 14;
        bool previousCompleted = true;

        for (int i = 0; i < totalModules; ++i)
        {
            int progress = 0;
            if (i < static_cast<int>(progresses.size()))
            {
                progress = progresses[i];
            }

            bool isLocked = !previousCompleted && (i > 0);
            if (progress < 100)
            {
                previousCompleted = false;
            }

            QVariantMap map;
            map["id"] = i + 1;
            map["title"] = (i < moduleTitles.size()) ? QString("Module %1: %2").arg(i + 1).arg(moduleTitles[i])
                                                     : QString("Module %1").arg(i + 1);
            map["progress"] = progress;
            map["isLocked"] = isLocked;

            m_modules.append(map);
        }

        emit modulesChanged();
    }

    void DashboardController::openModule(int moduleId)
    {
        loadRoadmap(moduleId);
        emit openRoadmapRequested(moduleId, m_currentModuleTitle);
    }

    void DashboardController::loadRoadmap(int moduleId)
    {
        m_currentModuleId = moduleId;
        m_currentModuleTitle = QString("Module %1").arg(moduleId);
        for (const auto &m : m_modules)
        {
            auto map = m.toMap();
            if (map["id"].toInt() == moduleId)
            {
                m_currentModuleTitle = map["title"].toString();
                break;
            }
        }

        m_roadmapNodes.clear();

        if (m_courseService && m_userId > 0)
        {
            auto nodes =
                m_courseService->getModuleRoadmap(static_cast<uint64_t>(m_userId), static_cast<uint64_t>(moduleId));
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                QVariantMap n;
                n["id"] = static_cast<int>(nodes[i].lessonId);
                n["index"] = static_cast<int>(i + 1);
                n["title"] = nodes[i].title;
                n["isCompleted"] = nodes[i].isCompleted;
                n["isLocked"] = nodes[i].isLocked;
                m_roadmapNodes.append(n);
            }
        }

        // Fallback default roadmap nodes if database is empty for this module
        if (m_roadmapNodes.isEmpty())
        {
            for (int i = 1; i <= 5; ++i)
            {
                QVariantMap n;
                n["id"] = (moduleId - 1) * 10 + i;
                n["index"] = i;
                n["title"] = QString("Lesson %1.%2: Key Concepts").arg(moduleId).arg(i);
                n["isCompleted"] = (i == 1 && moduleId == 1);
                n["isLocked"] = (i > 2 && moduleId == 1) || (moduleId > 1);
                m_roadmapNodes.append(n);
            }
        }

        emit roadmapChanged();
    }

    QStringList DashboardController::availableAvatars() const
    {
        return {"qrc:/images/default_avatar1.png", "qrc:/images/default_avatar2.png",
                "qrc:/images/default_avatar3.png", "qrc:/images/default_avatar4.png",
                "qrc:/images/walter.png",          "qrc:/images/tony.png",
                "qrc:/images/kanye.png",           "qrc:/images/terry.png",
                "qrc:/images/patrick.png",         "qrc:/images/lb.png"};
    }

    void DashboardController::updateAvatar(const QString &avatarPath)
    {
        m_avatarPath = avatarPath;
        if (m_userService && m_userId > 0)
        {
            m_userService->updateAvatar(static_cast<uint64_t>(m_userId), avatarPath);
        }
        emit userChanged();
    }

    void DashboardController::logout()
    {
        QSettings settings("CppForge", "StudyApp");
        settings.remove("auth/remember");
        settings.remove("auth/user_id");
        settings.remove("auth/username");
        settings.sync();

        emit logoutRequested();
    }
} // namespace cppforge::gui
