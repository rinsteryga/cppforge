#pragma once

#include "../../core/include/entities/CodingTask.hpp"
#include "../../core/include/services/CodeRunner.hpp"
#include "../../core/include/services/StaticAnalyzer.hpp"
#include "../../core/include/services/TaskManager.hpp"
#include "../../core/include/services/UserService.hpp"

#include <QObject>
#include <QQmlEngine>
#include <QString>

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
        Q_PROPERTY(QString title READ title NOTIFY taskChanged)
        Q_PROPERTY(QString description READ description NOTIFY taskChanged)
        Q_PROPERTY(QString theory READ theory NOTIFY taskChanged)
        Q_PROPERTY(QString initialCode READ initialCode NOTIFY taskChanged)
        Q_PROPERTY(QString outputLog READ outputLog NOTIFY outputLogChanged)
        Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
        Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY taskChanged)

    public:
        explicit TaskController(QObject *parent = nullptr);
        ~TaskController() override = default;

        void setUserService(cppforge::services::UserService *userService);
        Q_INVOKABLE void setUserId(int userId);

        int taskId() const
        {
            return m_taskId;
        }
        int lessonId() const
        {
            return m_lessonId;
        }
        QString title() const
        {
            return m_title;
        }
        QString description() const
        {
            return m_description;
        }
        QString theory() const
        {
            return m_theory;
        }
        QString initialCode() const
        {
            return m_initialCode;
        }
        QString outputLog() const
        {
            return m_outputLog;
        }
        bool isRunning() const
        {
            return m_isRunning;
        }
        bool isCompleted() const
        {
            return m_isCompleted;
        }

        Q_INVOKABLE void loadTask(int lessonId);
        Q_INVOKABLE void runCode(const QString &code);
        Q_INVOKABLE void submitSolution(const QString &code);
        Q_INVOKABLE void resetCode();
        Q_INVOKABLE void clearLog();

    signals:
        void taskChanged();
        void outputLogChanged();
        void isRunningChanged();
        void taskSolved();

    private:
        void appendLog(const QString &text);

        int m_userId = -1;
        cppforge::services::UserService *m_userService = nullptr;

        int m_taskId = 0;
        int m_lessonId = 0;
        QString m_title;
        QString m_description;
        QString m_theory;
        QString m_initialCode;
        QString m_outputLog;
        bool m_isRunning = false;
        bool m_isCompleted = false;

        cppforge::entities::CodingTask m_currentTask;
        std::unique_ptr<cppforge::services::CodeRunner> m_runner;
        std::unique_ptr<cppforge::services::StaticAnalyzer> m_analyzer;
    };
} // namespace cppforge::gui
