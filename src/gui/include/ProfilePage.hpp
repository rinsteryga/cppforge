#pragma once

#include "../../core/include/entities/Achievement.hpp"
#include "../../core/include/services/UserService.hpp"

#include <QWidget>

class QLabel;

class ProfilePage : public QWidget
{
    Q_OBJECT
public:
    explicit ProfilePage(QWidget *parent = nullptr);

    void setUserData(uint64_t id, const QString &name, const QString &avatarPath);
    void setUserService(cppforge::services::UserService *service) { userService_ = service; }

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