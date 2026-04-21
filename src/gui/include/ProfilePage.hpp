#pragma once

#include <QLabel>
#include <QString>
#include <QWidget>

namespace cppforge::services
{
    class UserService;
}

class ProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit ProfilePage(QWidget *parent = nullptr);

    void setUserData(int id, const QString &name, const QString &avatarPath = "");
    void setUserService(cppforge::services::UserService *service);

private slots:
    void onChangeAvatarClicked();
    void onAboutClicked();
    void onContactsClicked();
    void onPrivacyClicked();

private:
    void setupUI();
    void applyStyles();

    int currentUserId = -1;

    QLabel *avatarLabel = nullptr;
    QLabel *userNameLabel = nullptr;
    QLabel *solvedTasksLabel = nullptr;

    cppforge::services::UserService *userService_ = nullptr;
};