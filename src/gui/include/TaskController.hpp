#pragma once

#include "../../core/include/entities/CodingTask.hpp"
#include "../../core/include/services/AchievementService.hpp"
#include "../../core/include/services/CodeRunner.hpp"
#include "../../core/include/services/CourseService.hpp"
#include "../../core/include/services/StaticAnalyzer.hpp"
#include "../../core/include/services/TaskManager.hpp"
#include "../../core/include/services/UserService.hpp"

#include <QObject>
#include <QQmlEngine>
#include <QString>

#include <memory>
#include <optional>

namespace cppforge::gui
{
    /**
     * @class TaskController
     * @brief Manages task loading, code compilation, analysis, and submission.
     */
    class TaskController : public QObject
    {
        Q_OBJECT
        QML_ELEMENT

        Q_PROPERTY(int taskId READ taskId NOTIFY taskChanged)
        Q_PROPERTY(int lessonId READ lessonId NOTIFY taskChanged)
        Q_PROPERTY(int moduleId READ moduleId NOTIFY taskChanged)
        Q_PROPERTY(QString title READ title NOTIFY taskChanged)
        Q_PROPERTY(QString description READ description NOTIFY taskChanged)
        Q_PROPERTY(QString theory READ theory NOTIFY taskChanged)
        Q_PROPERTY(QString initialCode READ initialCode NOTIFY taskChanged)
        Q_PROPERTY(QString outputLog READ outputLog NOTIFY outputLogChanged)
        Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
        Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY taskChanged)
        Q_PROPERTY(bool hasCodingTask READ hasCodingTask NOTIFY taskChanged)
        Q_PROPERTY(bool hasNextLesson READ hasNextLesson NOTIFY taskChanged)
        Q_PROPERTY(bool hasPrevLesson READ hasPrevLesson NOTIFY taskChanged)
        Q_PROPERTY(int nextLessonId READ nextLessonId NOTIFY taskChanged)
        Q_PROPERTY(int prevLessonId READ prevLessonId NOTIFY taskChanged)

    public:
        explicit TaskController(QObject *parent = nullptr);
        ~TaskController() override = default;

        void setUserService(cppforge::services::UserService *userService);
        void setCourseService(cppforge::services::CourseService *courseService);
        void setAchievementService(cppforge::services::AchievementService *achievementService);

        Q_INVOKABLE void setUserId(int userId);

        int taskId() const { return m_taskId; }
        int lessonId() const { return m_lessonId; }
        int moduleId() const { return m_moduleId; }
        QString title() const { return m_title; }
        QString description() const { return m_description; }
        QString theory() const { return m_theory; }
        QString initialCode() const { return m_initialCode; }
        QString outputLog() const { return m_outputLog; }
        bool isRunning() const { return m_isRunning; }
        bool isCompleted() const { return m_isCompleted; }
        bool hasCodingTask() const { return m_hasCodingTask; }
        bool hasNextLesson() const { return m_nextLessonId > 0; }
        bool hasPrevLesson() const { return m_prevLessonId > 0; }
        int nextLessonId() const { return m_nextLessonId; }
        int prevLessonId() const { return m_prevLessonId; }

        Q_INVOKABLE void loadTask(int lessonId);
        Q_INVOKABLE void nextLesson();
        Q_INVOKABLE void prevLesson();
        Q_INVOKABLE void runCode(const QString &code);
        Q_INVOKABLE void submitSolution(const QString &code);
        Q_INVOKABLE void markTheoryStudied();
        Q_INVOKABLE void resetCode();
        Q_INVOKABLE void clearLog();

    signals:
        void taskChanged();
        void outputLogChanged();
        void isRunningChanged();
        void taskSolved();
        void lessonCompleted(int lessonId);
        void achievementUnlocked(const QString &name, const QString &description, const QString &iconPath);

    private:
        void appendLog(const QString &text);
        void saveTaskProgress(bool success, const QString &code);
        void checkAchievements();

        int m_userId = -1;
        cppforge::services::UserService *m_userService = nullptr;
        cppforge::services::CourseService *m_courseService = nullptr;
        cppforge::services::AchievementService *m_achievementService = nullptr;

        int m_taskId = 0;
        int m_lessonId = 0;
        int m_moduleId = 1;
        int m_nextLessonId = 0;
        int m_prevLessonId = 0;

        QString m_title;
        QString m_description;
        QString m_theory;
        QString m_initialCode;
        QString m_outputLog;
        bool m_isRunning = false;
        bool m_isCompleted = false;
        bool m_hasCodingTask = true;

        cppforge::entities::CodingTask m_currentTask;
        std::unique_ptr<cppforge::services::CodeRunner> m_runner;
        std::unique_ptr<cppforge::services::StaticAnalyzer> m_analyzer;
    };
} // namespace cppforge::gui
