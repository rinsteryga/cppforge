#include "TaskController.hpp"

#include <QThreadPool>

namespace cppforge::gui
{
    TaskController::TaskController(QObject *parent)
        : QObject(parent),
          m_runner(std::make_unique<cppforge::services::CodeRunner>(this)),
          m_analyzer(std::make_unique<cppforge::services::StaticAnalyzer>())
    {
    }

    void TaskController::setUserService(cppforge::services::UserService *userService)
    {
        m_userService = userService;
    }

    void TaskController::setUserId(int userId)
    {
        m_userId = userId;
    }

    void TaskController::loadTask(int lessonId)
    {
        m_lessonId = lessonId;
        m_taskId = lessonId;
        m_outputLog.clear();

        m_title = QString("Lesson %1: Coding Challenge").arg(lessonId);
        m_description =
            "### Task Objective\nImplement the solution according to specifications.\n\n```cpp\nint solve(int a, int "
            "b) {\n    return a + b;\n}\n```";
        m_theory =
            "### Theoretical Background\nFocus on memory safety, correct data types, and const correctness in modern "
            "C++.";
        m_initialCode =
            "#include <iostream>\n\nint main() {\n    std::cout << \"Solution ready\" << std::endl;\n    return 0;\n}\n";

        emit taskChanged();
        emit outputLogChanged();
    }

    void TaskController::appendLog(const QString &text)
    {
        m_outputLog += text + "\n";
        emit outputLogChanged();
    }

    void TaskController::clearLog()
    {
        m_outputLog.clear();
        emit outputLogChanged();
    }

    void TaskController::resetCode()
    {
        emit taskChanged();
    }

    void TaskController::runCode(const QString &code)
    {
        if (m_isRunning)
        {
            return;
        }

        m_isRunning = true;
        emit isRunningChanged();

        appendLog("[RUN] Compiling and running tests...");

        std::vector<cppforge::entities::TestCase> testCases;
        if (!m_currentTask.getTestCases().empty())
        {
            testCases.push_back(*m_currentTask.getTestCases().begin());
        }

        QThreadPool::globalInstance()->start([this, code, testCases]() {
            auto result = m_runner->runBlocking(code, testCases);
            QMetaObject::invokeMethod(this, [this, result]() {
                if (result.isSuccess() && result.getErrors().isEmpty())
                {
                    appendLog("[SUCCESS] Output:\n" + result.getOutput());
                }
                else
                {
                    appendLog("[ERROR]:\n" +
                              (result.getErrors().isEmpty() ? result.getOutput() : result.getErrors()));
                }
                m_isRunning = false;
                emit isRunningChanged();
            });
        });
    }

    void TaskController::submitSolution(const QString &code)
    {
        if (m_isRunning)
        {
            return;
        }

        m_isRunning = true;
        emit isRunningChanged();

        appendLog("[SUBMIT] Running full verification...");

        auto violation = m_analyzer->analyze(m_currentTask, code);
        if (violation.has_value())
        {
            appendLog("[STATIC ANALYSIS ERROR]: " + violation.value());
            m_isRunning = false;
            emit isRunningChanged();
            return;
        }

        std::vector<cppforge::entities::TestCase> allTests(m_currentTask.getTestCases().begin(),
                                                           m_currentTask.getTestCases().end());

        QThreadPool::globalInstance()->start([this, code, allTests]() {
            auto result = m_runner->runBlocking(code, allTests);
            QMetaObject::invokeMethod(this, [this, result]() {
                if (result.isSuccess() && result.getErrors().isEmpty())
                {
                    appendLog("✅ [ALL TESTS PASSED] Great job!");
                    m_isCompleted = true;
                    if (m_userService && m_userId > 0)
                    {
                        m_userService->saveLessonProgress(static_cast<uint64_t>(m_userId), 1,
                                                          static_cast<uint64_t>(m_lessonId), true);
                    }
                    emit taskSolved();
                }
                else
                {
                    appendLog("❌ [FAILED]:\n" +
                              (result.getErrors().isEmpty() ? result.getOutput() : result.getErrors()));
                }
                m_isRunning = false;
                emit isRunningChanged();
                emit taskChanged();
            });
        });
    }
} // namespace cppforge::gui
