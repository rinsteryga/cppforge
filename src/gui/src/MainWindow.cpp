#include "MainWindow.hpp"
#include "CustomTitleBar.hpp"
#include "ProfilePage.hpp"
#include "TaskWindow.hpp"

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
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , isTransitioning_(false)
    , pendingModuleId_(-1)
    , m_currentUsername("")
    , m_currentUserId(-1)
{
    setupUI();
    setWindowOpacity(0.0);
    
    QTimer::singleShot(50, this, &MainWindow::centerWindow);
    QTimer::singleShot(100, this, &MainWindow::fadeIn);
}

MainWindow::~MainWindow() = default;

void MainWindow::setUserId(int id)
{
    m_currentUserId = id;
    qDebug() << "MainWindow: ID пользователя установлен:" << m_currentUserId;
    
    loadAllModulesProgress();
}

void MainWindow::loadAllModulesProgress()
{
    if (m_currentUserId == -1 || moduleProgressBars.isEmpty()) return;

    for (int i = 1; i <= 14; ++i) {
        QSqlQuery query;
        query.prepare(R"(
            SELECT 
                (SELECT COUNT(*) FROM user_progress 
                 WHERE user_id = :uid AND module_id = :mid AND is_completed = TRUE) * 100 / 
                NULLIF((SELECT COUNT(*) FROM lessons WHERE module_id = :mid), 0)
        )");
        
        query.bindValue(":uid", m_currentUserId);
        query.bindValue(":mid", i);

        int progressValue = 0;
        if (query.exec() && query.next()) {
            progressValue = query.value(0).toInt();
        } else {
            qDebug() << "SQL Error Module" << i << ":" << query.lastError().text();
        }

        updateModuleProgress(i, progressValue);
    }
}

void MainWindow::updateModuleProgress(int moduleId, int progress)
{
    if (moduleId < 1 || moduleId > (int)moduleProgressBars.size()) {
        return;
    }

    moduleProgressBars[moduleId - 1]->setValue(progress);
    moduleProgressLabels[moduleId - 1]->setText(QString("%1% выполнено").arg(progress));

    if (progress == 100 && moduleId < (int)moduleButtons.size()) {
        QPushButton *nextBtn = moduleButtons[moduleId]; 
        if (nextBtn && !nextBtn->isEnabled()) {
            nextBtn->setEnabled(true);
            nextBtn->setText("Начать обучение");
            nextBtn->setStyleSheet(""); 
            
            disconnect(nextBtn, &QPushButton::clicked, nullptr, nullptr);
            connect(nextBtn, &QPushButton::clicked, this, &MainWindow::onModuleButtonClicked);
        }
    }
}

void MainWindow::onTaskWindowClosed()
{
    this->setWindowOpacity(0.0);
    this->show();
    
    loadAllModulesProgress();
    
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
    if (!isTransitioning_) {
        loadAllModulesProgress();
    }
}

void MainWindow::centerWindow()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect availableGeometry = screen->availableGeometry();
        int x = availableGeometry.x() + (availableGeometry.width() - width()) / 2;
        int y = availableGeometry.y() + (availableGeometry.height() - height()) / 2;
        move(x, y);
    }
}

void MainWindow::fadeIn()
{
    if (transitionAnimation_ && transitionAnimation_->state() == QPropertyAnimation::Running) {
        transitionAnimation_->stop();
    }

    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(300);
    transitionAnimation_->setStartValue(this->windowOpacity());
    transitionAnimation_->setEndValue(1.0);
    transitionAnimation_->setEasingCurve(QEasingCurve::InOutCubic);
    transitionAnimation_->start();
}

void MainWindow::fadeOut()
{
    if (transitionAnimation_ && transitionAnimation_->state() == QPropertyAnimation::Running) {
        transitionAnimation_->stop();
    }

    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(200);
    transitionAnimation_->setStartValue(1.0);
    transitionAnimation_->setEndValue(0.0);
    transitionAnimation_->setEasingCurve(QEasingCurve::InOutCubic);

    connect(transitionAnimation_.get(), &QPropertyAnimation::finished, this, [this]() {
        if (pendingModuleId_ != -1) {
            if (!taskWindow_) {
                taskWindow_ = std::make_unique<TaskWindow>();
                connect(taskWindow_.get(), &TaskWindow::moduleProgressUpdated, this, &MainWindow::updateModuleProgress);
                connect(taskWindow_.get(), &TaskWindow::windowClosed, this, &MainWindow::onTaskWindowClosed);
            }
            taskWindow_->setUserId(m_currentUserId);
            taskWindow_->loadModule(pendingModuleId_);
            this->hide();
            taskWindow_->show();
            taskWindow_->fadeIn();
            pendingModuleId_ = -1;
            isTransitioning_ = false;
        }
    });
    transitionAnimation_->start();
}

void MainWindow::setupWindowProperties()
{
    setFixedSize(1280, 900);
    setWindowTitle("cppforge - Main Menu");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
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
    sideBar->setFixedWidth(220);

    auto layout = new QVBoxLayout(sideBar.get());
    layout->setContentsMargins(20, 40, 20, 30);
    layout->setSpacing(12);

    auto logoContainer = new QFrame();
    logoContainer->setObjectName("logoContainer");
    auto logoLayout = new QVBoxLayout(logoContainer);
    
    auto logoIcon = new QLabel();
    logoIcon->setAlignment(Qt::AlignCenter);
    QPixmap logoPixmap(":/icons/main_logo.ico");
    if (!logoPixmap.isNull()) {
        logoIcon->setPixmap(logoPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    logoIcon->setFixedSize(100, 100);
    logoLayout->addWidget(logoIcon);
    
    learnBtn = new QPushButton("Учиться");
    ratingBtn = new QPushButton("Рейтинг");
    profileBtn = new QPushButton("Профиль");

    QFont btnFont("Roboto", 13, QFont::Medium);
    for (auto btn : {learnBtn, ratingBtn, profileBtn}) {
        btn->setFont(btnFont);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(48);
        btn->setObjectName("navButton");
        layout->addWidget(btn);
    }

    connect(learnBtn, &QPushButton::clicked, this, &MainWindow::onLearnButtonClicked);
    connect(profileBtn, &QPushButton::clicked, this, &MainWindow::onProfileButtonClicked);

    layout->insertWidget(0, logoContainer, 0, Qt::AlignCenter);
    layout->addStretch();
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
    
    auto eventTitle = new QLabel("События");
    eventTitle->setFont(QFont("Roboto", 18, QFont::Bold));
    eLayout->addWidget(eventTitle);
    eLayout->addWidget(new QLabel("Нет предстоящих событий"));
    eLayout->addStretch();

    dailyTaskCard = std::make_unique<QFrame>();
    dailyTaskCard->setProperty("class", "card");
    auto dLayout = new QVBoxLayout(dailyTaskCard.get());
    dLayout->setContentsMargins(25, 25, 25, 25);
    
    auto dailyTitle = new QLabel("Задание дня");
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

    for (int i = 1; i <= 14; ++i) {
        auto moduleCard = std::make_unique<QFrame>();
        moduleCard->setProperty("class", "card");
        auto mLayout = new QVBoxLayout(moduleCard.get());
        mLayout->setContentsMargins(20, 20, 20, 20);

        auto mTitle = new QLabel(QString("Модуль %1").arg(i));
        mTitle->setFont(QFont("Roboto", 15, QFont::Bold));

        auto progress = new QProgressBar();
        progress->setValue(0);
        progress->setFixedHeight(16);

        auto progressLabel = new QLabel("0% выполнено");
        progressLabel->setFont(QFont("Roboto", 12));

        bool isLocked = (i != 1);
        auto button = new QPushButton(isLocked ? "Заблокировано" : "Начать обучение");
        button->setProperty("moduleId", i);
        button->setFixedHeight(42);
        button->setCursor(Qt::PointingHandCursor);
        button->setEnabled(!isLocked);

        if (!isLocked) {
            connect(button, &QPushButton::clicked, this, &MainWindow::onModuleButtonClicked);
        } else {
            button->setStyleSheet("background: #f0f0f0; color: #999; border: 1px solid #e0e0e0;");
        }

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
    learningPage = new QWidget();

    auto learningLayout = new QHBoxLayout(learningPage);
    learningLayout->setContentsMargins(0, 0, 0, 0);
    learningLayout->setSpacing(30);

    auto eventWidget = new QWidget();
    eventWidget->setLayout(centerPanelLayout_.release());

    learningLayout->addWidget(modulesScrollArea.get(), 2);
    learningLayout->addWidget(eventWidget, 1);

    contentStack->addWidget(learningPage);
    contentStack->addWidget(profilePage);

    containerLayout->addWidget(sideBar.get(), 1);
    containerLayout->addWidget(contentStack.get(), 4);

    mainVerticalLayout->addWidget(contentContainer);
}

void MainWindow::setupStyles()
{
    setStyleSheet(R"(
        QWidget { background-color: #f5f7fb; font-family: 'Roboto'; }
        #MainWindow { background-color: white; border: 1px solid #d0d0d0; }
        QFrame#sideBar { background-color: white; border: 1px solid #e0e0e0; }
        QPushButton#navButton { background-color: transparent; border: none; color: #555; text-align: left; padding-left: 20px; }
        QPushButton#navButton:hover { background-color: #f0f2ff; color: #62639b; }
        QFrame[class="card"] { background-color: white; border: 1px solid #e0e0e0; border-radius: 8px; }
        QProgressBar { background: #eef0f5; border: 1px solid #ddd; text-align: center; color: #333; border-radius: 4px; }
        QProgressBar::chunk { background: #62639b; border-radius: 4px; }
        QPushButton { background: #62639b; color: white; border-radius: 4px; padding: 5px 15px; font-weight: bold; }
        QPushButton:hover { background: #51528a; }
        QPushButton:disabled { background: #f0f0f0; color: #999; }
    )");
}

void MainWindow::onModuleButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        int moduleId = button->property("moduleId").toInt();
        animateToTaskWindow(moduleId);
    }
}

void MainWindow::onLearnButtonClicked()
{
    contentStack->setCurrentIndex(0);
}

void MainWindow::onProfileButtonClicked()
{
    QSqlQuery query;
    query.prepare("SELECT id, username, avatar_path FROM users WHERE username = :name");
    query.bindValue(":name", m_currentUsername);

    if (query.exec() && query.next()) {
        int id = query.value("id").toInt();
        QString name = query.value("username").toString();
        QString avatar = query.value("avatar_path").toString();
        profilePage->setUserData(id, name, avatar);
        m_currentUserId = id;
    }
    contentStack->setCurrentIndex(1);
}

void MainWindow::animateToTaskWindow(int moduleId)
{
    if (isTransitioning_) return;
    isTransitioning_ = true;
    pendingModuleId_ = moduleId;
    fadeOut();
}

void MainWindow::openTaskWindow(int moduleId)
{
    animateToTaskWindow(moduleId);
}