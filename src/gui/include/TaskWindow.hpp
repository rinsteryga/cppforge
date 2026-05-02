#pragma once

#include "../../core/include/entities/CodingTask.hpp"
#include "../../core/include/services/CodeRunner.hpp"
#include "../../core/include/services/CourseService.hpp"
#include "../../core/include/services/StaticAnalyzer.hpp"
#include "../../core/include/services/ThemeService.hpp"
#include "../../core/include/services/UserService.hpp"
#include "CppHighlighter.hpp"
#include "CustomTitleBar.hpp"

#include <QDateTime>
#include <QDebug>
#include <QFutureWatcher>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextEdit>
#include <QWidget>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <memory>

class QFrame;

/**
 * @class TaskWindow
 * @brief Window for solving coding tasks and viewing theory.
 *
 * Provides a text editor for coding, a theory viewer, and an output console.
 * Integrated with CodeRunner and StaticAnalyzer for real-time feedback.
 */
class TaskWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a TaskWindow.
     * @param parent Parent widget.
     */
    explicit TaskWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~TaskWindow() override;

    /**
     * @brief Sets the user ID for progress tracking.
     * @param id User ID.
     */
    void setUserId(int64_t id);

    /**
     * @brief Loads a specific module/lesson.
     * @param lessonId Lesson ID.
     */
    void loadModule(int lessonId);

    /**
     * @brief Sets the active task.
     * @param task Task object.
     */
    void setTask(const cppforge::entities::CodingTask &task);

    /**
     * @brief Injects the user service.
     * @param service Pointer to UserService.
     */
    void setUserService(cppforge::services::UserService *service) { userService_ = service; }

    /**
     * @brief Injects the course service.
     * @param service Pointer to CourseService.
     */
    void setCourseService(cppforge::services::CourseService *service) { courseService_ = service; }

    /**
     * @brief Injects the theme service.
     * @param service Pointer to ThemeService.
     */
    void setThemeService(cppforge::services::ThemeService *service);

    /**
     * @brief Triggers fade-in animation.
     */
    void fadeIn();

    /**
     * @brief Triggers fade-out animation.
     */
    void fadeOut();

signals:
    /**
     * @brief Emitted when module progress changes.
     * @param id Module ID.
     * @param progress Progress percentage.
     */
    void moduleProgressUpdated(int id, int progress);

    /**
     * @brief Emitted when a lesson is successfully completed.
     * @param lessonId Lesson ID.
     */
    void lessonCompleted(int lessonId);

    /**
     * @brief Emitted when the window is closed.
     */
    void windowClosed();

    /**
     * @brief Emitted when a secret achievement is unlocked.
     * @param name Achievement name.
     */
    void customAchievementUnlocked(const QString &name);

protected:
    /**
     * @brief Handles window show event.
     * @param event Show event.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief Paints the window background.
     * @param event Paint event.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Filters events for the editor (e.g. Tab key).
     * @param obj Target object.
     * @param event Event object.
     * @return true if filtered.
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    /**
     * @brief Runs the current code against test cases.
     */
    void onRunClicked();

    /**
     * @brief Submits the current solution for final verification.
     */
    void onSubmitClicked();

    /**
     * @brief Switches to the next task in the module.
     */
    void onNextTask();

    /**
     * @brief Switches to the previous task in the module.
     */
    void onPrevTask();

private:
    /**
     * @brief Initializes the UI components.
     */
    void setupUI();

    /**
     * @brief Applies CSS styles based on the theme.
     */
    void setupStyles();

    /**
     * @brief Centers the window on screen.
     */
    void centerWindow();

    /**
     * @brief Saves task progress to the database.
     * @param success Whether the task was completed successfully.
     * @param code The submitted code.
     */
    void saveTaskProgress(bool success, const QString &code);

    /**
     * @brief Calculates total progress for a module.
     * @param moduleId Module ID.
     * @return Progress percentage (0-100).
     */
    int getModuleProgress(int moduleId);

    std::unique_ptr<CustomTitleBar> customTitleBar_;          ///< Custom title bar.
    std::unique_ptr<QPropertyAnimation> transitionAnimation_; ///< Transition animation.
    QStackedWidget *contentStack_{nullptr};                   ///< Page stack (Theory/Practice).
    QTextEdit *codeEditor_{nullptr};                          ///< Main code editor.
    QTextEdit *testOutput_{nullptr};                          ///< Test results console.
    QTextEdit *practiceEdit_{nullptr};                        ///< Practice task text.
    QTextEdit *theoryEdit_{nullptr};                          ///< Theory text viewer.

    QPushButton *btnNext_{nullptr};   ///< Next page button.
    QPushButton *btnPrev_{nullptr};   ///< Previous page button.
    QPushButton *btnBack_{nullptr};   ///< Back to main button.
    QPushButton *btnSubmit_{nullptr}; ///< Submit code button.
    QPushButton *btnRun_{nullptr};    ///< Run tests button.

    std::unique_ptr<cppforge::services::CodeRunner> runner_;       ///< Code execution service.
    std::unique_ptr<cppforge::services::StaticAnalyzer> analyzer_; ///< Static analysis service.

    cppforge::services::UserService *userService_{nullptr};     ///< User data service.
    cppforge::services::CourseService *courseService_{nullptr}; ///< Course content service.
    cppforge::services::ThemeService *themeService_{nullptr};   ///< Theme management service.

    std::unique_ptr<CppHighlighter> highlighter_; ///< Syntax highlighter for C++.
    cppforge::entities::CodingTask currentTask_;  ///< Currently active task.

    int64_t currentUserId_{-1};     ///< Logged in user ID.
    int currentModuleId_{-1};       ///< Active module ID.
    int currentModuleParentId_{-1}; ///< Parent module ID (for progress).
    bool hasCodingTask_{false};     ///< Flag if current page has a task.
    qint64 lastClickTime_{0};       ///< Throttle for button clicks.
};