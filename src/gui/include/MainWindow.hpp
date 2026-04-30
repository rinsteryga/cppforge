#pragma once

#include "DuelPage.hpp"
#include "DuelTaskWindow.hpp"
#include "ModuleRoadmapWidget.hpp"
#include "ProfilePage.hpp"
#include "TaskWindow.hpp"

#include <QEvent>
#include <QList>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlQuery>

#include <memory>
#include <vector>

namespace cppforge::services
{
    class UserService;
    class AchievementService;
    class CourseService;
} // namespace cppforge::services

class QFrame;
class QLabel;
class QPushButton;
class QProgressBar;
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class CustomTitleBar;

/**
 * @class MainWindow
 * @brief Main application window coordinating navigation between primary sections.
 *
 * MainWindow is the central component of the GUI.
 * It manages page switching (learning, profile, duels, module roadmaps),
 * integrates with business logic via services (UserService, AchievementService, CourseService),
 * and handles visual transition effects (fade-in/fade-out animations).
 */
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Main window constructor.
     * @param parent Parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor. Ensures proper resource release.
     */
    ~MainWindow();

    /**
     * @brief Sets the current authorized username for UI display.
     * @param username The user's name.
     */
    void setCurrentUser(const QString &username) { m_currentUsername = username; }

    /**
     * @brief Sets the user identifier in the system.
     * @param id Numeric user ID from the database.
     */
    void setUserId(int id);

    /**
     * @brief Injects the user service dependency.
     * @param service Pointer to UserService.
     */
    void setUserService(cppforge::services::UserService *service);

    /**
     * @brief Injects the achievement service dependency.
     * @param service Pointer to AchievementService.
     */
    void setAchievementService(cppforge::services::AchievementService *service);

    /**
     * @brief Injects the course service dependency.
     * @param service Pointer to CourseService.
     */
    void setCourseService(cppforge::services::CourseService *service);

protected:
    /**
     * @brief Overridden paint handler for custom backgrounds or effects.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Window show event handler. Used for initializing start animations.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief Handles window state changes (e.g., language or system theme changes).
     */
    void changeEvent(QEvent *event) override;

public slots:
    /**
     * @brief Starts the window fade-in animation (from transparency).
     */
    void fadeIn();

    /**
     * @brief Starts the window fade-out animation.
     */
    void fadeOut();

    /**
     * @brief Opens a specialized window for executing a specific task.
     * @param lessonId Identifier of the selected lesson/task.
     */
    void openTaskWindow(int lessonId);

private slots:
    /** @brief Module card click handler. */
    void onModuleButtonClicked();

    /** @brief Switches the interface to the main learning page. */
    void onLearnButtonClicked();

    /** @brief Switches the interface to the user profile page. */
    void onProfileButtonClicked();

    /** @brief Called when the task window is closed to return to the main interface. */
    void onTaskWindowClosed();

    /** @brief Handles user logout. */
    void onLogoutClicked();

    /**
     * @brief Updates visual progress indicators for a specific module.
     * @param moduleId Module ID.
     * @param progress Completion percentage (0-100).
     */
    void updateModuleProgress(int moduleId, int progress);

    /** @brief Returns from detailed module view to the module list. */
    void onBackToModulesClicked();

    /**
     * @brief Handles new achievement unlocked event.
     * @param achievement Earned achievement object.
     */
    void onAchievementUnlocked(cppforge::entities::Achievement achievement);
    void onSecretTaskTriggered();

private:
    /** @brief Initializes UI structure and creates pages. */
    void setupUI();

    /** @brief Configures window flags (borders, transparency, custom title). */
    void setupWindowProperties();

    /** @brief Configures the custom title bar. */
    void setupTitleBar();

    /** @brief Initializes the left navigation panel. */
    void setupLeftPanel();

    /** @brief Initializes the central content area (QStackedWidget). */
    void setupCenterPanel();

    /** @brief Initializes the right info panel. */
    void setupRightPanel();

    /** @brief Applies global QSS styles to all window elements. */
    void setupStyles();

    /** @brief Centers the application window on the screen. */
    void centerWindow();

    /**
     * @brief Starts an animated transition to a specific module's roadmap.
     * @param moduleId Target module ID.
     */
    void animateToTaskWindow(int moduleId);

    /** @brief Verifies user data in the DB before page initialization. */
    bool validateUserExists();

    /** @brief Loads progress for all available modules from the DB. */
    void loadAllModulesProgress();

    /**
     * @brief Builds the visual roadmap for the selected module.
     * @param moduleId Module ID to load.
     */
    void loadRoadmapForModule(int moduleId);

    std::unique_ptr<CustomTitleBar> customTitleBar_;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;
    std::unique_ptr<QVBoxLayout> centerPanelLayout_;
    std::unique_ptr<TaskWindow> taskWindow_;

    std::unique_ptr<QStackedWidget> contentStack;

    ProfilePage *profilePage{nullptr};
    QWidget *learningPage{nullptr};
    QWidget *roadmapPage{nullptr};
    DuelPage *duelPage{nullptr};
    DuelTaskWindow *m_duelTaskWindow = nullptr;

    QPushButton *learnBtn{nullptr};
    QPushButton *ratingBtn{nullptr};
    QPushButton *profileBtn{nullptr};
    QPushButton *logoutBtn{nullptr};

    ModuleRoadmapWidget *roadmapWidget{nullptr};

    QString m_currentUsername;
    int m_currentUserId{-1};
    int m_currentOpenModuleId{-1};

    cppforge::services::UserService *m_userService{nullptr};
    cppforge::services::AchievementService *m_achievementService{nullptr};
    cppforge::services::CourseService *m_courseService{nullptr};

    std::unique_ptr<QFrame> sideBar;
    std::unique_ptr<QFrame> eventCard;
    std::unique_ptr<QFrame> dailyTaskCard;
    std::unique_ptr<QScrollArea> modulesScrollArea;
    std::unique_ptr<QWidget> modulesContainer;
    std::unique_ptr<QVBoxLayout> modulesLayout;

    std::vector<std::unique_ptr<QFrame>> moduleCards;
    QList<QLabel *> moduleProgressLabels;
    QList<QProgressBar *> moduleProgressBars;
    QList<QPushButton *> moduleButtons;

    bool isTransitioning_{false};
    int pendingModuleId_{-1};
};