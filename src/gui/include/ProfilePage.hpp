#pragma once

#include <QLabel>
#include <QString>
#include <QWidget>

class ProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit ProfilePage(QWidget *parent = nullptr);

    void setUserData(int id, const QString &name, const QString &avatarPath = "");

private slots:
    void onChangeAvatarClicked();

private:
    void setupUI();
    void applyStyles();

    int currentUserId = -1;

    QLabel *avatarLabel = nullptr;
    QLabel *userNameLabel = nullptr;
};