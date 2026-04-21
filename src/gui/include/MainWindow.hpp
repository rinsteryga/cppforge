#pragma once

#include "ModuleRoadmapWidget.hpp"
#include "ProfilePage.hpp"
#include "TaskWindow.hpp"

#include <QList>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlQuery>

#include <memory>
#include <vector>

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

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

public slots:
    void fadeIn();
    void fadeOut();
    void openTaskWindow(int moduleId = 1);

private slots:
    void onModuleButtonClicked();
    void onLearnButtonClicked();
    void onProfileButtonClicked();
    void onTaskWindowClosed();
    void onLogoutClicked();
    void updateModuleProgress(int moduleId, int progress);

    void onBackToModulesClicked();

private:
    void setupUI();
    void setupWindowProperties();
    void setupTitleBar();
    void setupLeftPanel();
    void setupCenterPanel();
    void setupRightPanel();
    void setupStyles();
    void setupConnections();
    void centerWindow();
    void animateToTaskWindow(int moduleId);

    int m_currentOpenModuleId = -1;

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
    QPushButton *logoutBtn;
    ModuleRoadmapWidget *roadmapWidget{nullptr};

    QString m_currentUsername;
    int m_currentUserId{-1};

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

    std::unique_ptr<QHBoxLayout> footerLinksLayout;

    QPushButton *aboutBtn{nullptr};
    QPushButton *contactsBtn{nullptr};
    QPushButton *privacyBtn{nullptr};
    QPushButton *learnBtn{nullptr};
    QPushButton *profileBtn{nullptr};
    QPushButton *ratingBtn{nullptr};

    bool isTransitioning_{false};
    int pendingModuleId_{-1};
};