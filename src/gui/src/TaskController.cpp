#include "TaskController.hpp"

#include <QMetaObject>
#include <QThreadPool>

namespace cppforge::gui
{
    TaskController::TaskController(QObject *parent)
        : QObject(parent), m_runner(std::make_unique<cppforge::services::CodeRunner>(this)),
          m_analyzer(std::make_unique<cppforge::services::StaticAnalyzer>())
    {
    }

    void TaskController::setUserService(cppforge::services::UserService *userService)
    {
        m_userService = userService;
    }

    void TaskController::setCourseService(cppforge::services::CourseService *courseService)
    {
        m_courseService = courseService;
    }

    void TaskController::setAchievementService(cppforge::services::AchievementService *achievementService)
    {
        m_achievementService = achievementService;
        if (m_achievementService)
        {
            connect(m_achievementService, &cppforge::services::AchievementService::achievementUnlocked, this,
                    [this](const cppforge::entities::Achievement &ach)
                    { emit achievementUnlocked(ach.getName(), ach.getDescription(), ach.getIconPath()); });
        }
    }

    void TaskController::setUserId(int userId)
    {
        m_userId = userId;
    }

    void TaskController::loadTask(int lessonId)
    {
        m_lessonId = lessonId;
        m_outputLog.clear();

        if (m_courseService && m_userId > 0)
        {
            auto dataOpt =
                m_courseService->getTaskWindowData(static_cast<uint64_t>(m_userId), static_cast<uint64_t>(lessonId));
            if (dataOpt.has_value())
            {
                const auto &data = dataOpt.value();
                m_title = data.title;
                m_theory = data.theoryContent;
                m_isCompleted = data.isCompleted;
                m_moduleId = static_cast<int>(data.moduleId);
                m_hasCodingTask = data.taskId.has_value();

                if (m_hasCodingTask)
                {
                    m_taskId = static_cast<int>(data.taskId.value());
                    m_description = data.practiceDescription.value_or("");
                    m_initialCode = data.previousCode.value_or(data.initialCode.value_or(""));

                    m_currentTask = cppforge::entities::CodingTask(
                        static_cast<uint64_t>(m_taskId), static_cast<uint64_t>(lessonId), m_title, m_description,
                        data.initialCode.value_or(""), data.testCases, 1000, 256, data.whitelist, data.blacklist);
                }
                else
                {
                    m_taskId = 0;
                    m_description = "Practical tasks are not provided for this module. Study the theoretical material.";
                    m_initialCode = "// C++ Playground / Sandbox\n#include <iostream>\n\nint main() {\n    std::cout "
                                    "<< \"Hello from CppForge!\" << std::endl;\n    return 0;\n}\n";
                    m_currentTask = cppforge::entities::CodingTask(0, static_cast<uint64_t>(lessonId), m_title,
                                                                   m_description, m_initialCode, {}, 1000, 256, {}, {});
                }

                auto nextOpt = m_courseService->getNextLessonId(static_cast<uint64_t>(lessonId));
                m_nextLessonId = nextOpt.has_value() ? static_cast<int>(nextOpt.value()) : 0;

                auto prevOpt = m_courseService->getPrevLessonId(static_cast<uint64_t>(lessonId));
                m_prevLessonId = prevOpt.has_value() ? static_cast<int>(prevOpt.value()) : 0;

                if (m_isCompleted)
                {
                    appendLog("✓ Status: This task has already been completed.");
                }

                emit taskChanged();
                emit outputLogChanged();
                return;
            }
        }

        // Fallback for standalone/mock mode
        m_taskId = lessonId;
        m_title = QString("Lesson %1").arg(lessonId);
        m_theory = "Theory content is currently unavailable. Please verify database connection.";
        m_description = "Practice specifications.";
        m_initialCode = "// C++ Playground / Sandbox\n#include <iostream>\n\nint main() {\n    std::cout << \"Hello "
                        "from CppForge!\" << std::endl;\n    return 0;\n}\n";
        m_hasCodingTask = true;
        m_isCompleted = false;
        m_nextLessonId = 0;
        m_prevLessonId = 0;
        m_currentTask = cppforge::entities::CodingTask(static_cast<uint64_t>(lessonId), static_cast<uint64_t>(lessonId),
                                                       m_title, m_description, m_initialCode, {}, 1000, 256, {}, {});

        emit taskChanged();
        emit outputLogChanged();
    }

    void TaskController::nextLesson()
    {
        if (m_nextLessonId > 0)
        {
            loadTask(m_nextLessonId);
        }
    }

    void TaskController::prevLesson()
    {
        if (m_prevLessonId > 0)
        {
            loadTask(m_prevLessonId);
        }
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

        std::vector<cppforge::entities::TestCase> testCases;
        if (m_hasCodingTask && !m_currentTask.getTestCases().empty())
        {
            appendLog("[BUILD & RUN] Testing first test case...");
            testCases.push_back(*m_currentTask.getTestCases().begin());
        }
        else
        {
            appendLog("[BUILD & RUN] Compiling and running code in sandbox...");
        }

        QThreadPool::globalInstance()->start(
            [this, code, testCases]()
            {
                auto result = m_runner->runBlocking(code, testCases);
                QMetaObject::invokeMethod(
                    this,
                    [this, result]()
                    {
                        if (result.isSuccess() && result.getErrors().isEmpty())
                        {
                            if (result.getOutput().isEmpty())
                            {
                                appendLog("[SUCCESS] Process executed cleanly (exit code 0, empty output).");
                            }
                            else
                            {
                                appendLog("[SUCCESS] Output:\n" + result.getOutput());
                            }
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

        if (!m_hasCodingTask)
        {
            markTheoryStudied();
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

        QThreadPool::globalInstance()->start(
            [this, code, allTests]()
            {
                auto result = m_runner->runBlocking(code, allTests);
                QMetaObject::invokeMethod(this,
                                          [this, result, code]()
                                          {
                                              if (result.isSuccess() && result.getErrors().isEmpty())
                                              {
                                                  appendLog("✅ [ALL TESTS PASSED] Great job!");
                                                  m_isCompleted = true;
                                                  saveTaskProgress(true, code);
                                                  checkAchievements();
                                                  emit taskSolved();
                                              }
                                              else
                                              {
                                                  appendLog("❌ [FAILED]:\n" + (result.getErrors().isEmpty()
                                                                                    ? result.getOutput()
                                                                                    : result.getErrors()));
                                                  saveTaskProgress(false, code);
                                              }
                                              m_isRunning = false;
                                              emit isRunningChanged();
                                              emit taskChanged();
                                          });
            });
    }

    void TaskController::markTheoryStudied()
    {
        m_isCompleted = true;
        saveTaskProgress(true, "");
        checkAchievements();
        appendLog("✓ [THEORY COMPLETED] Progress saved.");
        emit taskSolved();
        emit taskChanged();
    }

    void TaskController::saveTaskProgress(bool success, const QString &code)
    {
        if (m_userService && m_userId > 0 && m_moduleId > 0 && m_lessonId > 0)
        {
            m_userService->saveLessonProgress(static_cast<uint64_t>(m_userId), static_cast<uint64_t>(m_moduleId),
                                              static_cast<uint64_t>(m_lessonId), success);

            if (m_hasCodingTask && !code.isEmpty())
            {
                m_userService->saveSubmission(static_cast<uint64_t>(m_userId), static_cast<uint64_t>(m_moduleId),
                                              static_cast<uint64_t>(m_taskId), code, success);
            }

            if (success)
            {
                emit lessonCompleted(m_lessonId);
            }
        }
    }

    void TaskController::checkAchievements()
    {
        if (m_achievementService && m_userService && m_userId > 0)
        {
            auto userOpt = m_userService->findById(static_cast<uint64_t>(m_userId));
            if (userOpt.has_value())
            {
                m_achievementService->checkAndAwardAchievements(*userOpt);
            }
        }
    }
} // namespace cppforge::gui
