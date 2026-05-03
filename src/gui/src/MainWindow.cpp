#include "MainWindow.hpp"

#include "../../core/include/services/AchievementService.hpp"
#include "../../core/include/services/CourseService.hpp"
#include "../../core/include/services/UserService.hpp"
#include "AchievementNotification.hpp"
#include "CustomTitleBar.hpp"
#include "DuelTaskWindow.hpp"
#include "ModuleRoadmapWidget.hpp"
#include "ProfilePage.hpp"
#include "TaskWindow.hpp"
#include "WindowStateManager.hpp"
#include "services/AchievementService.hpp"
#include "services/DuelManager.hpp"

#include <QApplication>
#include <QDebug>
#include <QFont>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QProcess>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), isTransitioning_(false), pendingModuleId_(-1), m_currentUsername(""), m_currentUserId(-1),
      m_currentOpenModuleId(-1)
{
    setupUI();
    setWindowOpacity(0.0);

    WindowStateManager::instance().applyGeometry(this, QSize(1400, 950));

    QTimer::singleShot(100, this, &MainWindow::fadeIn);
}

MainWindow::~MainWindow() = default;

bool MainWindow::validateUserExists()
{
    if (m_currentUserId <= 0)
        return false;

    if (!m_userService)
        return false;

    if (!m_userService->findById(m_currentUserId).has_value())
    {
        qDebug() << "!!! CRITICAL ERROR: User removed from DB. Resetting session.";

        QSettings settings("CppForge", "StudyApp");
        settings.remove("auth/user_id");
        settings.sync();

        m_currentUserId = -1;
        return false;
    }
    return true;
}

void MainWindow::setUserId(int id)
{
    m_currentUserId = id;

    if (!validateUserExists())
    {
        this->close();
        return;
    }

    if (m_userService)
    {
        auto userOpt = m_userService->findById(id);
        if (userOpt)
        {
            m_currentUsername = userOpt->getUsername();
            QString avatar = userOpt->getAvatarPath();

            if (profilePage)
            {
                profilePage->setUserData(id, m_currentUsername, avatar);
            }
            if (duelPage)
            {
                duelPage->setUserId(id);
                duelPage->setUserService(m_userService);

                double winrate = 0.0;
                int total = userOpt->getDuelWins() + userOpt->getDuelLosses();
                if (total > 0)
                {
                    winrate = (static_cast<double>(userOpt->getDuelWins()) / total) * 100.0;
                }

                duelPage->updateUserStats(m_currentUsername, userOpt->getDuelPoints(), winrate, avatar);
            }
        }
    }

    loadAllModulesProgress();
}

void MainWindow::setUserService(cppforge::services::UserService *service)
{
    m_userService = service;
    if (profilePage)
    {
        profilePage->setUserService(service);
    }
}

void MainWindow::setAchievementService(cppforge::services::AchievementService *service)
{
    m_achievementService = service;
    if (m_achievementService)
    {
        connect(m_achievementService, &cppforge::services::AchievementService::achievementUnlocked, this,
                &MainWindow::onAchievementUnlocked);
    }
}

void MainWindow::setCourseService(cppforge::services::CourseService *service)
{
    m_courseService = service;
}

void MainWindow::setThemeService(cppforge::services::ThemeService *service)
{
    m_themeService = service;
    if (m_themeService)
    {
        connect(
            m_themeService, &cppforge::services::ThemeService::themeChanged, this,
            [this](cppforge::services::Theme theme)
            {
                QString iconPath =
                    (theme == cppforge::services::Theme::Dark) ? ":/icons/main_logo_dark.ico" : ":/icons/main_logo.ico";
                if (customTitleBar_)
                {
                    customTitleBar_->setIcon(QIcon(iconPath));
                    customTitleBar_->setThemeService(m_themeService);
                }
                if (sideBarLogo_)
                {
                    QPixmap pix(iconPath);
                    if (!pix.isNull())
                    {
                        sideBarLogo_->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                }
                setupStyles();
            });

        QString initIcon = (m_themeService->getCurrentTheme() == cppforge::services::Theme::Dark)
                               ? ":/icons/main_logo_dark.ico"
                               : ":/icons/main_logo.ico";

        if (customTitleBar_)
        {
            customTitleBar_->setIcon(QIcon(initIcon));
            customTitleBar_->setThemeService(m_themeService);
        }
        if (sideBarLogo_)
        {
            QPixmap pix(initIcon);
            if (!pix.isNull())
            {
                sideBarLogo_->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    }

    if (profilePage)
    {
        profilePage->setThemeService(service);
    }
    if (duelPage)
    {
        duelPage->setThemeService(service);
    }
}

void MainWindow::loadAllModulesProgress()
{
    if (m_currentUserId == -1 || moduleProgressBars.isEmpty() || !m_courseService)
        return;

    std::vector<int> progresses = m_courseService->getAllModulesProgress(m_currentUserId);

    for (int i = 0; i < progresses.size(); ++i)
    {
        updateModuleProgress(i + 1, progresses[i]);
    }
}

void MainWindow::updateModuleProgress(int moduleId, int progress)
{
    if (moduleId < 1 || moduleId > (int)moduleProgressBars.size())
        return;

    moduleProgressBars[moduleId - 1]->setValue(progress);
    moduleProgressLabels[moduleId - 1]->setText(QString("%1% completed").arg(progress));

    if (progress == 100 && moduleId < (int)moduleButtons.size())
    {
        QPushButton *nextBtn = moduleButtons[moduleId];
        if (nextBtn && !nextBtn->isEnabled())
        {
            nextBtn->setEnabled(true);
            nextBtn->setText("Start Learning");
            nextBtn->setStyleSheet("");
            disconnect(nextBtn, &QPushButton::clicked, nullptr, nullptr);
            connect(nextBtn, &QPushButton::clicked, this, &MainWindow::onModuleButtonClicked);
        }
    }
}

void MainWindow::onTaskWindowClosed()
{
    this->setWindowOpacity(0.0);
    WindowStateManager::instance().applyState(this, QSize(1400, 950));
    loadAllModulesProgress();

    if (m_currentOpenModuleId != -1)
    {
        loadRoadmapForModule(m_currentOpenModuleId);
    }

    if (m_achievementService && m_userService && !m_currentUsername.isEmpty())
    {
        auto userOpt = m_userService->getUser(m_currentUsername);
        if (userOpt)
        {
            m_achievementService->checkAndAwardAchievements(*userOpt);
        }
    }

    fadeIn();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (!isTransitioning_)
        loadAllModulesProgress();
}

void MainWindow::centerWindow()
{
    QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
    if (!screen)
        screen = QGuiApplication::primaryScreen();

    if (screen)
    {
        QRect adjRect = screen->availableGeometry();
        move(adjRect.center() - rect().center());
    }
}

void MainWindow::fadeIn()
{
    if (transitionAnimation_ && transitionAnimation_->state() == QPropertyAnimation::Running)
        transitionAnimation_->stop();

    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(300);
    transitionAnimation_->setStartValue(this->windowOpacity());
    transitionAnimation_->setEndValue(1.0);
    transitionAnimation_->setEasingCurve(QEasingCurve::InOutCubic);
    transitionAnimation_->start();
}

void MainWindow::fadeOut()
{
    if (transitionAnimation_ && transitionAnimation_->state() == QPropertyAnimation::Running)
        transitionAnimation_->stop();

    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(200);
    transitionAnimation_->setStartValue(1.0);
    transitionAnimation_->setEndValue(0.0);
    transitionAnimation_->setEasingCurve(QEasingCurve::InOutCubic);

    connect(transitionAnimation_.get(), &QPropertyAnimation::finished, this,
            [this]()
            {
                if (pendingModuleId_ != -1)
                {
                    if (!taskWindow_)
                    {
                        taskWindow_ = std::make_unique<TaskWindow>();
                        if (taskWindow_)
                        {
                            taskWindow_->setUserService(m_userService);
                            taskWindow_->setCourseService(m_courseService);
                            taskWindow_->setThemeService(m_themeService);
                        }
                        connect(taskWindow_.get(), &TaskWindow::moduleProgressUpdated, this,
                                &MainWindow::updateModuleProgress);
                        connect(taskWindow_.get(), &TaskWindow::windowClosed, this, &MainWindow::onTaskWindowClosed);
                        connect(taskWindow_.get(), &TaskWindow::customAchievementUnlocked, this,
                                [this](const QString &name)
                                {
                                    if (m_achievementService && m_userService && m_currentUserId > 0)
                                    {
                                        auto userOpt = m_userService->findById(m_currentUserId);
                                        if (userOpt)
                                        {
                                            m_achievementService->awardCustomEvent(*userOpt, name);
                                        }
                                    }
                                });
                    }
                    taskWindow_->setUserId(m_currentUserId);
                    taskWindow_->loadModule(pendingModuleId_);
                    WindowStateManager::instance().captureState(this);
                    this->hide();
                    WindowStateManager::instance().applyState(taskWindow_.get(), QSize(1300, 900));
                    taskWindow_->fadeIn();
                    pendingModuleId_ = -1;
                    isTransitioning_ = false;
                }
            });
    transitionAnimation_->start();
}

void MainWindow::setupWindowProperties()
{
    setMinimumSize(1100, 800);
    resize(1400, 950);
    setWindowTitle("cppforge - Main Menu");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setObjectName("MainWindow");
}

void MainWindow::setupTitleBar()
{
    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setTitle("cppforge - Main Menu");
    customTitleBar_->setIcon(windowIcon());
}

void MainWindow::setupLeftPanel()
{
    sideBar = std::make_unique<QFrame>(this);
    sideBar->setObjectName("sideBar");
    sideBar->setFixedWidth(240);

    auto layout = new QVBoxLayout(sideBar.get());
    layout->setContentsMargins(20, 40, 20, 30);
    layout->setSpacing(12);

    auto logoContainer = new QFrame();
    logoContainer->setObjectName("logoContainer");
    logoContainer->setStyleSheet("background: transparent;");
    auto logoLayout = new QVBoxLayout(logoContainer);

    sideBarLogo_ = new QLabel();
    sideBarLogo_->setAlignment(Qt::AlignCenter);
    sideBarLogo_->setStyleSheet("background: transparent;");

    QString logoPath = ":/icons/main_logo.ico";
    if (m_themeService && m_themeService->getCurrentTheme() == cppforge::services::Theme::Dark)
        logoPath = ":/icons/main_logo_dark.ico";

    QPixmap logoPixmap(logoPath);
    if (!logoPixmap.isNull())
        sideBarLogo_->setPixmap(logoPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    sideBarLogo_->setFixedSize(100, 100);
    logoLayout->addWidget(sideBarLogo_);
    layout->addWidget(logoContainer, 0, Qt::AlignCenter);
    layout->addSpacing(20);

    learnBtn = new QPushButton("Learn");
    ratingBtn = new QPushButton("Duel");
    profileBtn = new QPushButton("Profile");
    logoutBtn = new QPushButton("Logout");

    QFont btnFont("Roboto", 13, QFont::Medium);
    for (auto btn : {learnBtn, ratingBtn, profileBtn, logoutBtn})
    {
        btn->setFont(btnFont);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(52);
        btn->setObjectName("navButton");
    }

    logoutBtn->setStyleSheet("QPushButton#navButton { color: #d9534f; } "
                             "QPushButton#navButton:hover { background-color: #fff1f0; color: #d9534f; }");

    layout->addWidget(learnBtn);
    layout->addWidget(ratingBtn);
    layout->addWidget(profileBtn);

    layout->addStretch();

    layout->addWidget(logoutBtn);

    connect(learnBtn, &QPushButton::clicked, this, &MainWindow::onLearnButtonClicked);
    connect(profileBtn, &QPushButton::clicked, this, &MainWindow::onProfileButtonClicked);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);

    connect(ratingBtn, &QPushButton::clicked, this,
            [this]()
            {
                if (duelPage)
                {
                    contentStack->setCurrentWidget(duelPage);
                }
            });
}

void MainWindow::setupCenterPanel()
{
    centerPanelLayout_ = std::make_unique<QVBoxLayout>();
    centerPanelLayout_->setContentsMargins(0, 0, 0, 0);
    centerPanelLayout_->setSpacing(20);
    centerPanelLayout_->setAlignment(Qt::AlignTop);

    eventCard = std::make_unique<QFrame>();
    eventCard->setProperty("class", "card");
    auto eLayout = new QVBoxLayout(eventCard.get());
    eLayout->setContentsMargins(25, 25, 25, 25);

    auto eventTitle = new QLabel("Events");
    eventTitle->setFont(QFont("Roboto", 18, QFont::Bold));
    eLayout->addWidget(eventTitle);
    eLayout->addWidget(new QLabel("No upcoming events"));
    eLayout->addStretch();

    dailyTaskCard = std::make_unique<QFrame>();
    dailyTaskCard->setProperty("class", "card");
    auto dLayout = new QVBoxLayout(dailyTaskCard.get());
    dLayout->setContentsMargins(25, 25, 25, 25);

    auto dailyTitle = new QLabel("Daily Task");
    dailyTitle->setFont(QFont("Roboto", 18, QFont::Bold));
    dLayout->addWidget(dailyTitle);

    auto dailyProgress = new QProgressBar();
    dailyProgress->setFixedHeight(16);
    dailyProgress->setValue(0);
    dLayout->addWidget(dailyProgress);
    dLayout->addStretch();

    centerPanelLayout_->addWidget(eventCard.get());
    centerPanelLayout_->addWidget(dailyTaskCard.get());
}

void MainWindow::setupRightPanel()
{
    modulesScrollArea = std::make_unique<QScrollArea>();
    modulesScrollArea->setObjectName("modulesScrollArea");
    modulesScrollArea->setWidgetResizable(true);
    modulesScrollArea->setFrameStyle(QFrame::NoFrame);

    modulesContainer = std::make_unique<QWidget>();
    modulesLayout = std::make_unique<QVBoxLayout>(modulesContainer.get());
    modulesLayout->setContentsMargins(0, 0, 0, 0);
    modulesLayout->setSpacing(15);

    for (int i = 1; i <= 14; ++i)
    {
        auto moduleCard = std::make_unique<QFrame>();
        moduleCard->setProperty("class", "card");
        auto mLayout = new QVBoxLayout(moduleCard.get());
        mLayout->setContentsMargins(20, 20, 20, 20);

        auto mTitle = new QLabel(QString("Module %1").arg(i));
        mTitle->setFont(QFont("Roboto", 15, QFont::Bold));

        auto progress = new QProgressBar();
        progress->setValue(0);
        progress->setFixedHeight(16);

        auto progressLabel = new QLabel("0% completed");
        progressLabel->setFont(QFont("Roboto", 12));

        bool isLocked = (i != 1);
        auto button = new QPushButton(isLocked ? "Locked" : "Start Learning");
        button->setProperty("moduleId", i);
        button->setFixedHeight(45);
        button->setCursor(Qt::PointingHandCursor);
        button->setEnabled(!isLocked);

        if (!isLocked)
            connect(button, &QPushButton::clicked, this, &MainWindow::onModuleButtonClicked);
        else
            button->setStyleSheet("background: #f0f0f0; color: #999; border: 1px solid #e0e0e0;");

        mLayout->addWidget(mTitle);
        mLayout->addWidget(progress);
        mLayout->addWidget(progressLabel);
        mLayout->addWidget(button, 0, Qt::AlignRight);

        moduleProgressBars.append(progress);
        moduleProgressLabels.append(progressLabel);
        moduleButtons.append(button);

        modulesLayout->addWidget(moduleCard.get());
        moduleCards.push_back(std::move(moduleCard));
    }
    modulesLayout->addStretch();
    modulesScrollArea->setWidget(modulesContainer.release());
}

void MainWindow::setupUI()
{
    setupWindowProperties();
    setupTitleBar();
    setupStyles();

    auto mainVerticalLayout = new QVBoxLayout(this);
    mainVerticalLayout->setContentsMargins(0, 0, 0, 0);
    mainVerticalLayout->setSpacing(0);
    mainVerticalLayout->addWidget(customTitleBar_.get());

    auto contentContainer = new QWidget();
    contentContainer->setObjectName("contentContainer");
    auto containerLayout = new QHBoxLayout(contentContainer);
    containerLayout->setContentsMargins(30, 30, 30, 30);
    containerLayout->setSpacing(30);

    setupLeftPanel();
    setupRightPanel();
    setupCenterPanel();

    contentStack = std::make_unique<QStackedWidget>();
    profilePage = new ProfilePage(this);
    connect(profilePage, &ProfilePage::secretTaskTriggered, this, &MainWindow::onSecretTaskTriggered);
    connect(profilePage, &ProfilePage::avatarChanged, this,
            [this](const QString &path)
            {
                if (duelPage)
                {
                    auto userOpt = m_userService->getUser(m_currentUsername);
                    if (userOpt)
                    {
                        double winrate = 0.0;
                        int total = userOpt->getDuelWins() + userOpt->getDuelLosses();
                        if (total > 0)
                        {
                            winrate = (static_cast<double>(userOpt->getDuelWins()) / total) * 100.0;
                        }
                        duelPage->updateUserStats(m_currentUsername, userOpt->getDuelPoints(), winrate, path);
                    }
                }
            });

    learningPage = new QWidget();
    duelPage = new DuelPage();
    duelPage->setUserId(m_currentUserId);
    duelPage->setUserService(m_userService);
    connect(duelPage, &DuelPage::startDuelSession, this,
            [this](const cppforge::entities::CodingTask &task)
            {
                auto manager = duelPage->getDuelManager();

                manager->sendIdentity(m_currentUsername);

                m_duelTaskWindow = new DuelTaskWindow(manager);
                m_duelTaskWindow->setThemeService(m_themeService);

                m_duelTaskWindow->setTask(task);

                m_duelTaskWindow->setLocalNickname(m_currentUsername);

                m_duelTaskWindow->setAttribute(Qt::WA_DeleteOnClose);

                connect(manager, &cppforge::services::DuelManager::duelFinished, m_duelTaskWindow,
                        &DuelTaskWindow::showFinalResult);

                connect(m_duelTaskWindow, &DuelTaskWindow::sessionClosed, this,
                        [this]()
                        {
                            WindowStateManager::instance().applyState(this, QSize(1400, 950));
                            this->fadeIn();
                        });

                connect(m_duelTaskWindow, &QWidget::destroyed, this,
                        [this]()
                        {
                            m_duelTaskWindow = nullptr;
                            WindowStateManager::instance().applyState(this, QSize(1400, 950));
                            this->fadeIn();
                        });

                WindowStateManager::instance().captureState(this);
                this->hide();
                WindowStateManager::instance().applyState(m_duelTaskWindow, QSize(1300, 900));
            });

    roadmapPage = new QWidget();
    auto roadmapLayout = new QVBoxLayout(roadmapPage);
    roadmapLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *backBtn = new QPushButton("← Back to Modules");
    backBtn->setMinimumWidth(200);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::onBackToModulesClicked);

    QScrollArea *roadmapScroll = new QScrollArea();
    roadmapScroll->setWidgetResizable(true);
    roadmapScroll->setFrameStyle(QFrame::NoFrame);
    roadmapScroll->setStyleSheet("background: transparent;");

    roadmapWidget = new ModuleRoadmapWidget();
    connect(roadmapWidget, &ModuleRoadmapWidget::lessonSelected, this, &MainWindow::openTaskWindow);

    roadmapScroll->setWidget(roadmapWidget);

    roadmapLayout->addWidget(backBtn, 0, Qt::AlignLeft | Qt::AlignTop);
    roadmapLayout->addWidget(roadmapScroll);

    auto learningLayout = new QHBoxLayout(learningPage);
    learningLayout->setContentsMargins(0, 0, 0, 0);
    learningLayout->setSpacing(30);

    auto eventWidget = new QWidget();
    eventWidget->setLayout(centerPanelLayout_.release());

    learningLayout->addWidget(modulesScrollArea.get(), 2);
    learningLayout->addWidget(eventWidget, 1);

    contentStack->addWidget(learningPage);
    contentStack->addWidget(profilePage);
    contentStack->addWidget(roadmapPage);
    contentStack->addWidget(duelPage);

    containerLayout->addWidget(sideBar.get(), 1);
    containerLayout->addWidget(contentStack.get(), 4);

    mainVerticalLayout->addWidget(contentContainer);
}

void MainWindow::setupStyles()
{
    bool isDark = (palette().color(QPalette::Window).lightness() < 128);

    QString hoverColor, accentColor;
    QString hoverText = "white";

    if (isDark)
    {
        hoverColor = "#0e639c";
        accentColor = "#0e639c";
    }
    else
    {
        hoverColor = "#f3e8ff";
        accentColor = "#62639b";
        hoverText = "black";
    }

    setStyleSheet(QString(R"(
        QWidget { background-color: palette(alternate-base); font-family: 'Roboto'; color: palette(text); }
        #MainWindow { background-color: palette(base); border: 1px solid palette(mid); }
        
        QFrame#sideBar { background-color: palette(base); border: 1px solid palette(mid); }
        
        QLabel { background-color: transparent; border: none; }

        QPushButton#navButton { background-color: transparent; border: none; color: palette(window-text); text-align: left; padding-left: 20px; }
        QPushButton#navButton:hover { background-color: %1; color: %2; }
        
        QFrame[class="card"] { background-color: palette(base); border: 1px solid palette(mid); border-radius: 8px; }
        
        QProgressBar { background: palette(alternate-base); border: 1px solid palette(mid); text-align: center; color: palette(text); border-radius: 4px; }
        QProgressBar::chunk { background: %3; border-radius: 4px; }
        
        QPushButton { background: palette(button); color: palette(button-text); border: 1px solid palette(mid); border-radius: 4px; padding: 5px 15px; font-weight: bold; }
        QPushButton:hover { background: %1; color: %2; }
        QPushButton#logoutButton:hover { background: palette(link); color: palette(highlighted-text); }
        QPushButton:disabled { background: palette(disabled, button); color: palette(disabled, button-text); }
    )")
                      .arg(hoverColor)
                      .arg(hoverText)
                      .arg(accentColor));

    style()->unpolish(this);
    style()->polish(this);
    update();
}

void MainWindow::onModuleButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button && roadmapWidget && roadmapPage)
    {
        int moduleId = button->property("moduleId").toInt();
        loadRoadmapForModule(moduleId);
        contentStack->setCurrentWidget(roadmapPage);
    }
}

void MainWindow::onLearnButtonClicked()
{
    contentStack->setCurrentIndex(0);
}

void MainWindow::onProfileButtonClicked()
{
    if (m_userService)
    {
        auto userOpt = m_userService->getUser(m_currentUsername);
        if (userOpt)
        {
            int id = userOpt->getId();
            QString name = userOpt->getUsername();
            QString avatar = userOpt->getAvatarPath();
            profilePage->setUserData(id, name, avatar);
            m_currentUserId = id;
        }
    }
    contentStack->setCurrentIndex(1);
}

void MainWindow::animateToTaskWindow(int moduleId)
{
    if (isTransitioning_)
        return;
    isTransitioning_ = true;
    pendingModuleId_ = moduleId;
    fadeOut();
}

void MainWindow::openTaskWindow(int lessonId)
{
    animateToTaskWindow(lessonId);
}

void MainWindow::loadRoadmapForModule(int moduleId)
{
    if (m_currentUserId == -1 || !m_courseService)
        return;

    m_currentOpenModuleId = moduleId;

    auto nodesData = m_courseService->getModuleRoadmap(m_currentUserId, moduleId);

    std::vector<RoadmapNode> nodes;
    for (const auto &data : nodesData)
    {
        RoadmapNode node;
        node.data = data;
        nodes.push_back(node);
    }

    roadmapWidget->setLessons(nodes);

    QTimer::singleShot(50,
                       [this]()
                       {
                           QScrollArea *scroll = roadmapPage->findChild<QScrollArea *>();
                           if (scroll && scroll->verticalScrollBar())
                           {
                               scroll->verticalScrollBar()->setValue(scroll->verticalScrollBar()->maximum());
                           }
                       });
}

void MainWindow::onBackToModulesClicked()
{
    if (contentStack)
    {
        contentStack->setCurrentIndex(0);
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        if (this->isMinimized())
        {
            event->accept();
            return;
        }

        if (this->isMaximized())
        {
        }
        else
        {
            this->setContentsMargins(0, 0, 0, 0);
        }
    }
    QWidget::changeEvent(event);
}

void MainWindow::onLogoutClicked()
{
    QSettings settings("CppForge", "StudyApp");
    settings.remove("auth/remember");
    settings.remove("auth/user_id");
    settings.remove("auth/username");

    settings.sync();

    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void MainWindow::onAchievementUnlocked(cppforge::entities::Achievement achievement)
{
    qDebug() << "[MainWindow] Slot achievementUnlocked triggered for:" << achievement.getName();
    QWidget *activeWin = QApplication::activeWindow();
    if (!activeWin)
    {
        activeWin = this;
    }

    auto *notif = new cppforge::gui::AchievementNotification(achievement.getName(), achievement.getDescription(),
                                                             achievement.getIconPath(), activeWin);
    notif->showAnimated();

    if (contentStack->currentWidget() == profilePage)
    {
        qDebug() << "[MainWindow] Refreshing profile page...";
        onProfileButtonClicked();
    }
}

void MainWindow::onSecretTaskTriggered()
{
    qDebug() << "[MainWindow] Secret task triggered!";

    QString desc =
        "ОАО \"Экскаваторный завод 'Ковровец'\"\n\n"
        "Некоторые заготовки завода изготавливаются в заготовительном цехе на раскройном оборудовании с ЧПУ.\n"
        "Отдел главного конструктора создает чертежи деталей и сохраняет их в формате dxf. "
        "После этого чертежи передаются по сети в отдел главного металлурга, там формируют размещение "
        "набора деталей на листе (раскрой), вычисляют коэффициент использования металла (КИМ, то есть отношение "
        "массы детали к массе заготовки). После этого по сети чертежи попадают в бюро ЧПУ заготовительного цеха, "
        "где разрабатываются программы для раскроя. По сети программа передается оператору станка.\n\n"
        "Требуется:\n"
        "- максимально возможный КИМ (по базе раскроя);\n"
        "- сколько полученных чертежей не обработано отделом главного металлурга.\n\n"
        "Формат ввода:\n"
        "N (количество чертежей)\n"
        "Для каждого чертежа строка: <Статус> <Масса детали> <Масса заготовки>\n"
        "Статусы: 'Создан', 'Обработан', 'В_бюро_ЧПУ'. Необработанные чертежи имеют статус 'Создан'.\n\n"
        "Формат вывода:\n"
        "1 строка: максимальный КИМ (округленный до 2 знаков после запятой, например, 0.85). Если нет обработанных, "
        "вывести 0.00.\n"
        "2 строка: количество необработанных чертежей.\n";

    QString initCode = "#include <iostream>\n#include <string>\n#include <vector>\n#include <iomanip>\n\n"
                       "using namespace std;\n\n"
                       "int main() {\n"
                       "    \n"
                       "    return 0;\n"
                       "}\n";
    std::set<cppforge::entities::TestCase> testCases;
    testCases.emplace(1, "3\nСоздан 10.0 15.0\nОбработан 12.0 15.0\nВ_бюро_ЧПУ 8.0 10.0\n", "0.80\n1", true);
    testCases.emplace(2, "2\nСоздан 5.0 10.0\nСоздан 3.0 4.0\n", "0.00\n2", true);
    testCases.emplace(3, "4\nОбработан 9.9 10.0\nОбработан 5.0 5.5\nВ_бюро_ЧПУ 100.0 101.0\nСоздан 1.0 2.0\n",
                      "0.99\n1", true);

    cppforge::entities::CodingTask secretTask(9999, std::nullopt, "Секретное задание: ОАО «Ковровец»", desc, initCode,
                                              testCases, 1000, 256);

    if (!taskWindow_)
    {
        taskWindow_ = std::make_unique<TaskWindow>();
        if (m_userService)
        {
            taskWindow_->setUserService(m_userService);
        }
        if (m_courseService)
        {
            taskWindow_->setCourseService(m_courseService);
        }
        connect(taskWindow_.get(), &TaskWindow::moduleProgressUpdated, this, &MainWindow::updateModuleProgress);
        connect(taskWindow_.get(), &TaskWindow::windowClosed, this, &MainWindow::onTaskWindowClosed);
        connect(taskWindow_.get(), &TaskWindow::customAchievementUnlocked, this,
                [this](const QString &name)
                {
                    if (m_achievementService && m_userService && m_currentUserId > 0)
                    {
                        auto userOpt = m_userService->findById(m_currentUserId);
                        if (userOpt)
                        {
                            m_achievementService->awardCustomEvent(*userOpt, name);
                        }
                    }
                });
    }

    taskWindow_->setUserId(m_currentUserId);
    taskWindow_->setTask(secretTask);

    this->hide();
    taskWindow_->show();
    taskWindow_->fadeIn();
}
