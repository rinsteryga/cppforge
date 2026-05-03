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
    class ThemeService;
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
    ~MainWindow() override;

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

    /**
     * @brief Injects the theme service dependency.
     * @param service Pointer to ThemeService.
     */
    void setThemeService(cppforge::services::ThemeService *service);

protected:
    /**
     * @brief Overridden paint handler for custom backgrounds or effects.
     * @param event Paint event.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Window show event handler. Used for initializing start animations.
     * @param event Show event.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief Handles window state changes (e.g., language or system theme changes).
     * @param event Change event.
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
    /**
     * @brief Module card click handler.
     */
    void onModuleButtonClicked();

    /**
     * @brief Switches the interface to the main learning page.
     */
    void onLearnButtonClicked();

    /**
     * @brief Switches the interface to the user profile page.
     */
    void onProfileButtonClicked();

    /**
     * @brief Called when the task window is closed to return to the main interface.
     */
    void onTaskWindowClosed();

    /**
     * @brief Handles user logout.
     */
    void onLogoutClicked();

    /**
     * @brief Updates visual progress indicators for a specific module.
     * @param moduleId Module ID.
     * @param progress Completion percentage (0-100).
     */
    void updateModuleProgress(int moduleId, int progress);

    /**
     * @brief Returns from detailed module view to the module list.
     */
    void onBackToModulesClicked();

    /**
     * @brief Handles new achievement unlocked event.
     * @param achievement Earned achievement object.
     */
    void onAchievementUnlocked(cppforge::entities::Achievement achievement);

    /**
     * @brief Secret shortcut trigger handler.
     */
    void onSecretTaskTriggered();

private:
    /**
     * @brief Initializes UI structure and creates pages.
     */
    void setupUI();

    /**
     * @brief Configures window flags (borders, transparency, custom title).
     */
    void setupWindowProperties();

    /**
     * @brief Configures the custom title bar.
     */
    void setupTitleBar();

    /**
     * @brief Initializes the left navigation panel.
     */
    void setupLeftPanel();

    /**
     * @brief Initializes the central content area (QStackedWidget).
     */
    void setupCenterPanel();

    /**
     * @brief Initializes the right info panel.
     */
    void setupRightPanel();

    /**
     * @brief Applies global QSS styles to all window elements.
     */
    void setupStyles();

    /**
     * @brief Centers the application window on the screen.
     */
    void centerWindow();

    /**
     * @brief Starts an animated transition to a specific module's roadmap.
     * @param moduleId Target module ID.
     */
    void animateToTaskWindow(int moduleId);

    /**
     * @brief Verifies user data in the DB before page initialization.
     * @return true if user exists, false otherwise.
     */
    bool validateUserExists();

    /**
     * @brief Loads progress for all available modules from the DB.
     */
    void loadAllModulesProgress();

    /**
     * @brief Builds the visual roadmap for the selected module.
     * @param moduleId Module ID to load.
     */
    void loadRoadmapForModule(int moduleId);

    std::unique_ptr<CustomTitleBar> customTitleBar_;          ///< Custom title bar.
    std::unique_ptr<QPropertyAnimation> transitionAnimation_; ///< Transition animation.
    std::unique_ptr<QVBoxLayout> centerPanelLayout_;          ///< Main center layout.
    std::unique_ptr<TaskWindow> taskWindow_;                  ///< Task execution window.
    std::unique_ptr<QStackedWidget> contentStack;             ///< Stack of central pages.

    ProfilePage *profilePage{nullptr};          ///< User profile page.
    QWidget *learningPage{nullptr};             ///< Main learning dashboard.
    QWidget *roadmapPage{nullptr};              ///< Module roadmap view.
    DuelPage *duelPage{nullptr};                ///< Duels/Network lobby page.
    DuelTaskWindow *m_duelTaskWindow = nullptr; ///< Active duel window.

    QPushButton *learnBtn{nullptr};   ///< Sidebar learn button.
    QPushButton *ratingBtn{nullptr};  ///< Sidebar duels button.
    QPushButton *profileBtn{nullptr}; ///< Sidebar profile button.
    QPushButton *logoutBtn{nullptr};  ///< Sidebar logout button.
    QLabel *sideBarLogo_{nullptr};    ///< Sidebar logo.

    ModuleRoadmapWidget *roadmapWidget{nullptr}; ///< Roadmap visual widget.

    QString m_currentUsername;     ///< Current authenticated username.
    int m_currentUserId{-1};       ///< Current user numeric ID.
    int m_currentOpenModuleId{-1}; ///< Currently active module.

    cppforge::services::UserService *m_userService{nullptr};               ///< User data service.
    cppforge::services::AchievementService *m_achievementService{nullptr}; ///< Achievement service.
    cppforge::services::CourseService *m_courseService{nullptr};           ///< Course/content service.
    cppforge::services::ThemeService *m_themeService{nullptr};             ///< Theme service.

    std::unique_ptr<QFrame> sideBar;                ///< Sidebar container.
    std::unique_ptr<QFrame> eventCard;              ///< UI card for events.
    std::unique_ptr<QFrame> dailyTaskCard;          ///< UI card for daily tasks.
    std::unique_ptr<QScrollArea> modulesScrollArea; ///< Scroll area for module list.
    std::unique_ptr<QWidget> modulesContainer;      ///< Container for module cards.
    std::unique_ptr<QVBoxLayout> modulesLayout;     ///< Layout for module cards.

    std::vector<std::unique_ptr<QFrame>> moduleCards; ///< List of module visual cards.
    QList<QLabel *> moduleProgressLabels;             ///< Labels for module percentage.
    QList<QProgressBar *> moduleProgressBars;         ///< Bars for module percentage.
    QList<QPushButton *> moduleButtons;               ///< Buttons to enter modules.

    bool isTransitioning_{false}; ///< Animation state flag.
    int pendingModuleId_{-1};     ///< Module waiting for animation end.
};