#pragma once

#include "../../core/include/entities/Achievement.hpp"
#include "../../core/include/services/UserService.hpp"

#include <QWidget>

class QLabel;

/**
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

signals:
    void secretTaskTriggered();

private slots:
    void onChangeAvatarClicked();
    void onAboutClicked();
    void onContactsClicked();
    void onPrivacyClicked();

private:
    void setupUI();
    void applyStyles();

    uint64_t currentUserId = 0;
    QLabel *avatarLabel = nullptr;
    QLabel *userNameLabel = nullptr;
    QLabel *solvedTasksLabel = nullptr;
    QLabel *completedLessonsLabel = nullptr;
    QLabel *achievementsLabel = nullptr;
    QLabel *totalSubmissionsLabel = nullptr;
    QLabel *streakLabel = nullptr;

    QWidget *achievementsContainer = nullptr;
    QWidget *activityContainer = nullptr;

    cppforge::services::UserService *userService_ = nullptr;
};