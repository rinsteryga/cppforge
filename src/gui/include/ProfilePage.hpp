#pragma once

#include "../../core/include/entities/Achievement.hpp"
#include "../../core/include/services/ThemeService.hpp"
#include "../../core/include/services/UserService.hpp"

#include <QWidget>

class QLabel;
class QComboBox;

namespace cppforge::services
{
    class AchievementService;
}

/**
 * @class ProfilePage
 * @brief Represents the user profile and statistics page.
 *
 * This widget displays the user's avatar, username, and key performance metrics
 * such as solved tasks, completed lessons, achievements, and current streak.
 * It also provides access to personal settings and activity history.
 */
class ProfilePage : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a new ProfilePage instance.
     * @param parent The parent QWidget.
     */
    explicit ProfilePage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~ProfilePage() override;

    /**
     * @brief Updates the UI with specific user data.
     * @param id The unique identifier of the user.
     * @param name The display name/username.
     * @param avatarPath The path to the user's avatar image.
     */
    void setUserData(uint64_t id, const QString &name, const QString &avatarPath);

    /**
     * @brief Injects the user service for data retrieval.
     * @param service Pointer to the UserService implementation.
     */
    void setUserService(cppforge::services::UserService *service) { userService_ = service; }

    /**
     * @brief Injects the achievement service for handling rewards.
     * @param service Pointer to the AchievementService.
     */
    void setAchievementService(cppforge::services::AchievementService *service);

    /**
     * @brief Injects the theme service for UI customization.
     * @param service Pointer to the ThemeService.
     */
    void setThemeService(cppforge::services::ThemeService *service);

signals:
    /**
     * @brief Emitted when a secret keyboard shortcut is activated.
     */
    void secretTaskTriggered();

    /**
     * @brief Emitted when the user selects a new avatar.
     * @param path The filesystem path to the new avatar image.
     */
    void avatarChanged(const QString &path);

private slots:
    /**
     * @brief Opens a file dialog to select a new profile picture.
     */
    void onChangeAvatarClicked();

    /**
     * @brief Displays information about the application.
     */
    void onAboutClicked();

    /**
     * @brief Shows contact information for support.
     */
    void onContactsClicked();

    /**
     * @brief Displays the privacy policy dialog.
     */
    void onPrivacyClicked();

    /**
     * @brief Shows information about hot keys.
     */
    void onHotKeysClicked();

    /**
     * @brief Handles theme selection from the dropdown menu.
     * @param index The selected index in the combobox.
     */
    void onThemeChanged(int index);

private:
    /**
     * @brief Initializes the UI components.
     */
    void setupUI();

    /**
     * @brief Applies CSS styles based on the current theme.
     */
    void applyStyles();

    /**
     * @brief Event filter for custom tooltip handling.
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

    /**
     * @brief Updates the avatar image with a circular mask.
     * @param path Path to the avatar image.
     */
    void updateAvatarDisplay(const QString &path);

    uint64_t currentUserId = 0;              ///< ID of the user whose profile is shown.
    QLabel *avatarLabel = nullptr;           ///< Avatar display label.
    QWidget *contentWidget_ = nullptr;       ///< Inner container for scrolling.
    QLabel *userNameLabel = nullptr;         ///< Username display label.
    QLabel *solvedTasksLabel = nullptr;      ///< Count of solved tasks.
    QLabel *completedLessonsLabel = nullptr; ///< Count of completed lessons.
    QLabel *achievementsLabel = nullptr;     ///< Count of achievements.
    QLabel *totalSubmissionsLabel = nullptr; ///< Count of total submissions.
    QLabel *duelPointsLabel = nullptr;       ///< User's rating points.
    QLabel *winRateLabel = nullptr;          ///< User's winrate percentage.
    QLabel *streakLabel = nullptr;           ///< User's current day streak.

    QWidget *achievementsContainer = nullptr; ///< Layout for achievement icons.
    QWidget *activityContainer = nullptr;     ///< Layout for activity history.

    cppforge::services::UserService *userService_ = nullptr;               ///< User data service.
    cppforge::services::AchievementService *achievementService_ = nullptr; ///< Achievement service.
    cppforge::services::ThemeService *themeService_ = nullptr;             ///< Theme service.
    QComboBox *themeCombo_ = nullptr;                                      ///< Theme selection combobox.
    QLabel *customTooltipLabel_ = nullptr; ///< Custom floating tooltip for achievements.
};