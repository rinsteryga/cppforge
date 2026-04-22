#pragma once

#include "DuelPage.hpp"
#include "ModuleRoadmapWidget.hpp"
#include "ProfilePage.hpp"
#include "TaskWindow.hpp"

#include <QList>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlQuery>
#include <QEvent>

#include <memory>
#include <vector>

namespace cppforge::services
{
    class UserService;
    class AchievementService;
} // namespace cppforge::services

class QFrame;
class QLabel;
class QPushButton;
class QProgressBar;
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class CustomTitleBar;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setCurrentUser(const QString &username) { m_currentUsername = username; }
    void setUserId(int id);
    void setUserService(cppforge::services::UserService *service);
    void setAchievementService(cppforge::services::AchievementService *service);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    
    void changeEvent(QEvent *event) override;

public slots:
    void fadeIn();
    void fadeOut();
    void openTaskWindow(int lessonId);

private slots:
    void onModuleButtonClicked();
    void onLearnButtonClicked();
    void onProfileButtonClicked();
    void onTaskWindowClosed();
    void onLogoutClicked();
    void updateModuleProgress(int moduleId, int progress);
    void onBackToModulesClicked();
    void onAchievementUnlocked(cppforge::entities::Achievement achievement);

private:
    void setupUI();
    void setupWindowProperties();
    void setupTitleBar();
    void setupLeftPanel();
    void setupCenterPanel();
    void setupRightPanel();
    void setupStyles();
    void centerWindow();
    void animateToTaskWindow(int moduleId);

    bool validateUserExists();

    void loadAllModulesProgress();
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