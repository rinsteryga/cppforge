#pragma once

#include "../../core/include/entities/CodingTask.hpp"
#include "../../core/include/services/CodeRunner.hpp"
#include "../../core/include/services/StaticAnalyzer.hpp"
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

namespace cppforge
{
    namespace services
    {
        class DuelManager;
    }
} // namespace cppforge

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
     * @brief Displays a modal window or panel with finished duel results.
     * @param winnerName Winner's name.
     * @param score Winner's final score.
     */
    void showFinalResult(const QString &winnerName, int score);

protected:
    /**
     * @brief Overridden window show event. Used for starting animations or the timer.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief Event filter for handling specific key presses in the editor (e.g., Tab).
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    /**
     * @brief Called every second. Updates the timer and dynamically decreases the current score.
     */
    void onTick();

    /**
     * @brief Executes the current code against the task's test suite.
     */
    void onRunClicked();

    /**
     * @brief Finalizes the solution. Verifies all tests and, if successful, signals duel completion.
     */
    void onSubmitClicked();

private:
    /** @brief Initializes UI components and their layout. */
    void setupUI();

    /** @brief Configures visual window style, fonts, and color scheme. */
    void setupStyles();

    QString m_localNickname; ///< Nickname of the current user.

    /** @brief Pointer to the network interaction manager. Does not own the object. */
    cppforge::services::DuelManager *duelManager_{nullptr};

    std::unique_ptr<CustomTitleBar> customTitleBar_;
    QTextEdit *codeEditor_{nullptr};
    QTextEdit *testOutput_{nullptr};
    QTextEdit *practiceEdit_{nullptr};

    QLabel *labelTimer_{nullptr};
    QLabel *labelScore_{nullptr};
    QTimer *duelTimer_{nullptr};

    int timeLeft_{600};
    int currentScore_{1000};

    cppforge::entities::CodingTask currentTask_;
    std::unique_ptr<cppforge::services::CodeRunner> runner_;
    std::unique_ptr<cppforge::services::StaticAnalyzer> analyzer_;
};