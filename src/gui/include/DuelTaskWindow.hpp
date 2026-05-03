#pragma once

#include "../../core/include/entities/CodingTask.hpp"
#include "../../core/include/services/CodeRunner.hpp"
#include "../../core/include/services/StaticAnalyzer.hpp"
#include "../../core/include/services/ThemeService.hpp"
#include "CppHighlighter.hpp"
#include "CustomTitleBar.hpp"

#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

namespace cppforge::services
{
    class DuelManager;
}

/**
 * @class DuelTaskWindow
 * @brief Window for an active duel session, combining a code editor, timer, and testing system.
 *
 * This class is the primary player interface during a duel. It is responsible for:
 * - Displaying task requirements and code writing.
 * - Indicating remaining time and current potential score.
 * - Running solution tests and static code analysis.
 * - Interacting with DuelManager to synchronize progress with the opponent.
 */
class DuelTaskWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Duel task window constructor.
     * @param manager Pointer to an existing DuelManager for network data exchange.
     * @param parent Parent widget.
     */
    explicit DuelTaskWindow(cppforge::services::DuelManager *manager, QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~DuelTaskWindow() override;

    /**
     * @brief Initializes the window with specific task data.
     * @param task Task object (text, initial code, tests).
     */
    void setTask(const cppforge::entities::CodingTask &task);

    /**
     * @brief Sets the local player's name for UI display.
     * @param name User's nickname.
     */
    void setLocalNickname(const QString &name);

    /**
     * @brief Connects the task window to the theme service for synchronized visual updates.
     * @param service Pointer to the global theme manager.
     */
    void setThemeService(cppforge::services::ThemeService *service);

    /**
     * @brief Displays the final results of the duel.
     * @param winnerName The name of the winner.
     * @param score Final score.
     */
    void showFinalResult(const QString &winnerName, int score);

signals:
    /**
     * @brief Emitted when the duel session is closed.
     */
    void sessionClosed();

protected:
    /**
     * @brief Overridden window show event.
     * @param event Show event data.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief Overridden paint event for custom rendering.
     * @param event Paint event data.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Event filter for handling specific key presses in the editor.
     * @param obj Target object.
     * @param event Event object.
     * @return true if filtered.
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    /**
     * @brief Called every second to update the timer and score.
     */
    void onTick();

    /**
     * @brief Executes the current code against the task's test suite.
     */
    void onRunClicked();

    /**
     * @brief Finalizes the solution and verifies all tests.
     */
    void onSubmitClicked();

    /**
     * @brief Signals that the user surrenders the duel.
     */
    void onSurrenderClicked();

    /**
     * @brief Closes the duel window and returns to the main interface.
     */
    void onExitClicked();

private:
    /**
     * @brief Initializes UI components and their layout.
     */
    void setupUI();

    /**
     * @brief Configures visual window style, fonts, and color scheme.
     * @param isDark Optional override for the theme state.
     */
    void setupStyles(std::optional<bool> isDark = std::nullopt);

    QString m_localNickname;                                ///< Nickname of the local user.
    cppforge::services::DuelManager *duelManager_{nullptr}; ///< Pointer to network manager.
    std::unique_ptr<CustomTitleBar> customTitleBar_;        ///< Custom title bar.
    QTextEdit *codeEditor_{nullptr};                        ///< Source code editor.
    QTextEdit *testOutput_{nullptr};                        ///< Test output console.
    QTextEdit *practiceEdit_{nullptr};                      ///< Task description.
    QPushButton *btnRun_{nullptr};                          ///< Run button.
    QPushButton *btnSubmit_{nullptr};                       ///< Submit button.
    QPushButton *btnSurrender_{nullptr};                    ///< Surrender button.
    QPushButton *btnExit_{nullptr};                         ///< Exit button.
    QLabel *labelTimer_{nullptr};                           ///< Timer display label.
    QLabel *labelScore_{nullptr};                           ///< Score display label.
    QTimer *duelTimer_{nullptr};                            ///< Countdown timer.

    int timeLeft_{600};      ///< Seconds remaining.
    int currentScore_{1000}; ///< Current potential points.

    cppforge::entities::CodingTask currentTask_;                   ///< Active duel task.
    cppforge::services::ThemeService *themeService_{nullptr};      ///< Theme service.
    std::unique_ptr<CppHighlighter> highlighter_;                  ///< Syntax highlighter.
    std::unique_ptr<cppforge::services::CodeRunner> runner_;       ///< Code execution engine.
    std::unique_ptr<cppforge::services::StaticAnalyzer> analyzer_; ///< Code analysis tool.
};