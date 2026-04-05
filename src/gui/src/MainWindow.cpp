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
    : QWidget(parent), isTransitioning_(false), pendingModuleId_(-1), m_currentUsername(""), m_currentUserId(-1)
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
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (!isTransitioning_)
    {
        setWindowOpacity(0.0);
        fadeIn();
    }
}

void MainWindow::centerWindow()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QRect availableGeometry = screen->availableGeometry();
        int x = availableGeometry.x() + (availableGeometry.width() - width()) / 2;
        int y = availableGeometry.y() + (availableGeometry.height() - height()) / 2;
        move(x, y);
    }
}

void MainWindow::fadeIn()
{
    if (transitionAnimation_ && transitionAnimation_->state() == QPropertyAnimation::Running)
        transitionAnimation_->stop();

    setWindowOpacity(0.0);
    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(300);
    transitionAnimation_->setStartValue(0.0);
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
                        connect(taskWindow_.get(), &TaskWindow::moduleProgressUpdated, this,
                                &MainWindow::updateModuleProgress);
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

    auto layout = std::make_unique<QVBoxLayout>();
    layout->setContentsMargins(20, 40, 20, 30);
    layout->setSpacing(12);

    auto logoContainer = std::make_unique<QFrame>();
    logoContainer->setObjectName("logoContainer");
    auto logoLayout = std::make_unique<QVBoxLayout>();
    logoLayout->setContentsMargins(0, 0, 0, 0);

    auto logoIcon = std::make_unique<QLabel>();
    logoIcon->setAlignment(Qt::AlignCenter);
    QPixmap logoPixmap(":/icons/main_logo.ico");

    if (!logoPixmap.isNull())
    {
        logoPixmap = logoPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        logoIcon->setPixmap(logoPixmap);
    }
    else
    {
        logoIcon->setText("CppForge");
        logoIcon->setStyleSheet("color: #62639b; font-size: 24px; font-weight: bold;");
    }
    logoIcon->setFixedSize(100, 100);

    logoLayout->addWidget(logoIcon.release());
    logoContainer->setLayout(logoLayout.release());

    learnBtn = new QPushButton("Учиться");
    ratingBtn = new QPushButton("Рейтинг");
    profileBtn = new QPushButton("Профиль");

    QFont btnFont("Roboto", 13, QFont::Medium);
    for (auto btn : {learnBtn, ratingBtn, profileBtn})
    {
        btn->setFont(btnFont);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(48);
        btn->setObjectName("navButton");
    }

    connect(learnBtn, &QPushButton::clicked, this, &MainWindow::onLearnButtonClicked);
    connect(profileBtn, &QPushButton::clicked, this, &MainWindow::onProfileButtonClicked);

    layout->addWidget(logoContainer.release(), 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(learnBtn);
    layout->addWidget(ratingBtn);
    layout->addWidget(profileBtn);
    layout->addStretch();

    sideBar->setLayout(layout.release());
}

void MainWindow::setupCenterPanel()
{
    centerPanelLayout_ = std::make_unique<QVBoxLayout>();
    centerPanelLayout_->setContentsMargins(0, 0, 0, 0);
    centerPanelLayout_->setSpacing(20);
    centerPanelLayout_->setAlignment(Qt::AlignTop);

    eventCard = std::make_unique<QFrame>();
    eventCard->setProperty("class", QVariant("card"));
    eventCard->setObjectName("eventCard");

    auto eLayout = std::make_unique<QVBoxLayout>();
    eLayout->setContentsMargins(25, 25, 25, 25);
    eLayout->setSpacing(15);

    auto eventTitle = std::make_unique<QLabel>("События");
    eventTitle->setProperty("class", "section-title");
    eventTitle->setFont(QFont("Roboto", 18, QFont::Bold));
    eventTitle->setStyleSheet("color: #333;");

    auto eventPlaceholder = std::make_unique<QLabel>("Нет предстоящих событий");
    eventPlaceholder->setObjectName("eventPlaceholder");
    eventPlaceholder->setAlignment(Qt::AlignCenter);
    eventPlaceholder->setFixedHeight(80);

    eLayout->addWidget(eventTitle.release());
    eLayout->addWidget(eventPlaceholder.release());
    eLayout->addStretch();
    eventCard->setLayout(eLayout.release());

    dailyTaskCard = std::make_unique<QFrame>();
    dailyTaskCard->setProperty("class", QVariant("card"));
    dailyTaskCard->setObjectName("dailyTaskCard");

    auto dLayout = std::make_unique<QVBoxLayout>();
    dLayout->setContentsMargins(25, 25, 25, 25);
    dLayout->setSpacing(15);

    auto dailyHeader = std::make_unique<QHBoxLayout>();
    auto dailyTitle = std::make_unique<QLabel>("Задание дня");
    dailyTitle->setFont(QFont("Roboto", 18, QFont::Bold));
    dailyTitle->setStyleSheet("color: #333;");

    auto dailyProgressText = std::make_unique<QLabel>("0/1");
    dailyProgressText->setObjectName("dailyProgressText");
    dailyProgressText->setFont(QFont("Roboto", 16, QFont::Bold));
    dailyProgressText->setAlignment(Qt::AlignRight);
    dailyProgressText->setStyleSheet("color: #333;");

    dailyHeader->addWidget(dailyTitle.release());
    dailyHeader->addWidget(dailyProgressText.release());

    auto dailyDesc = std::make_unique<QLabel>("Выполнить 1 задание");
    dailyDesc->setObjectName("dailyDesc");
    dailyDesc->setFont(QFont("Roboto", 14));
    dailyDesc->setStyleSheet("color: #555; font-weight: 500;");

    auto dailyProgress = std::make_unique<QProgressBar>();
    dailyProgress->setValue(0);
    dailyProgress->setFixedHeight(16);
    dailyProgress->setObjectName("dailyProgress");

    dLayout->addLayout(dailyHeader.release());
    dLayout->addWidget(dailyDesc.release());
    dLayout->addWidget(dailyProgress.release());
    dailyTaskCard->setLayout(dLayout.release());

    auto footerWidget = std::make_unique<QWidget>();
    footerWidget->setObjectName("footerWidget");
    footerLinksLayout = std::make_unique<QHBoxLayout>(footerWidget.get());
    footerLinksLayout->setContentsMargins(5, 20, 5, 0);
    footerLinksLayout->setSpacing(15);

    auto createLink = [](const QString &text)
    {
        auto btn = new QPushButton(text);
        btn->setProperty("class", "footer-link");
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };

    aboutBtn = createLink("О CppForge");
    contactsBtn = createLink("Контакты");
    privacyBtn = createLink("Конфиденциальность");

    footerLinksLayout->addWidget(aboutBtn);
    footerLinksLayout->addWidget(contactsBtn);
    footerLinksLayout->addWidget(privacyBtn);
    footerLinksLayout->addStretch();

    centerPanelLayout_->addSpacing(10);
    centerPanelLayout_->addWidget(eventCard.get());
    centerPanelLayout_->addWidget(dailyTaskCard.get());
    centerPanelLayout_->addStretch();
    centerPanelLayout_->addWidget(footerWidget.release());
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

    QFont titleFont("Roboto", 15, QFont::Bold);
    QFont statusFont("Roboto", 12);
    QFont btnFont("Roboto", 12, QFont::Medium);

    for (int i = 1; i <= 14; ++i)
    {
        auto moduleCard = std::make_unique<QFrame>();
        moduleCard->setProperty("class", QVariant("card"));

        auto mLayout = std::make_unique<QVBoxLayout>();
        mLayout->setContentsMargins(20, 20, 20, 20);
        mLayout->setSpacing(12);

        auto mTitle = std::make_unique<QLabel>(QString("Модуль %1").arg(i));
        mTitle->setFont(titleFont);
        mTitle->setStyleSheet("color: #333;");

        auto progress = new QProgressBar();
        progress->setValue(0);
        progress->setFixedHeight(16);
        progress->setObjectName("moduleProgress");

        auto progressLabel = new QLabel("0% выполнено");
        progressLabel->setFont(statusFont);
        progressLabel->setProperty("class", "progress-text");

        bool isLocked = (i != 1);
        auto button = new QPushButton(isLocked ? "Заблокировано" : "Начать обучение");
        button->setObjectName("moduleBtn");
        button->setProperty("moduleId", i);
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedHeight(42);
        button->setEnabled(!isLocked);

        if (!isLocked)
        {
            connect(button, &QPushButton::clicked, this, &MainWindow::onModuleButtonClicked);
        }
        else
        {
            button->setStyleSheet("background: #f0f0f0; color: #999; border: 1px solid #e0e0e0;");
        }

        mLayout->addWidget(mTitle.release());
        mLayout->addWidget(progress);
        mLayout->addWidget(progressLabel);
        mLayout->addWidget(button, 0, Qt::AlignRight);

        moduleCard->setLayout(mLayout.release());

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

    auto mainVerticalLayout = std::make_unique<QVBoxLayout>();
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

    auto *learningLayout = new QHBoxLayout(learningPage);
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
    setLayout(mainVerticalLayout.release());
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
        QPushButton#moduleBtn { background: #62639b; color: white; font-weight: 600; border-radius: 4px; border: none; padding: 8px 20px; }
        QPushButton#moduleBtn:hover { background: #7B7CB5; }
        QPushButton#moduleBtn:disabled { background: #f0f0f0; color: #999; border: 1px solid #e0e0e0; }
        QProgressBar { background: #eef0f5; border: 1px solid #ddd; text-align: center; color: #333; }
        QProgressBar::chunk { background: #62639b; }
    )");
}

void MainWindow::onModuleButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button)
    {
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
    qDebug() << "MainWindow: Пытаюсь открыть профиль для пользователя:" << m_currentUsername;

    if (m_currentUsername.isEmpty())
    {
        qWarning() << "MainWindow: Имя пользователя пусто!";
    }

    QSqlQuery query;
    query.prepare("SELECT id, username, avatar_path FROM users WHERE username = :name");
    query.bindValue(":name", m_currentUsername);

    if (query.exec() && query.next())
    {
        int id = query.value("id").toInt();
        QString name = query.value("username").toString();
        QString avatar = query.value("avatar_path").toString();

        qDebug() << "MainWindow: Данные найдены. ID:" << id;
        profilePage->setUserData(id, name, avatar);
        m_currentUserId = id;
    }
    else
    {
        qWarning() << "MainWindow: Пользователь не найден в БД или ошибка запроса:" << query.lastError().text();
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

void MainWindow::openTaskWindow(int moduleId)
{
    animateToTaskWindow(moduleId);
}

void MainWindow::onTaskWindowClosed()
{
    this->setWindowOpacity(0.0);
    this->show();
    fadeIn();
}

void MainWindow::updateModuleProgress(int moduleId, int progress)
{
    if (moduleId < 1 || moduleId > (int)moduleProgressBars.size())
        return;
    moduleProgressBars[moduleId - 1]->setValue(progress);
    moduleProgressLabels[moduleId - 1]->setText(QString("%1% выполнено").arg(progress));

    if (progress == 100 && moduleId < (int)moduleButtons.size())
    {
        QPushButton *nextBtn = moduleButtons[moduleId];
        if (nextBtn && !nextBtn->isEnabled())
        {
            nextBtn->setEnabled(true);
            nextBtn->setText("Начать обучение");
            nextBtn->setStyleSheet("");
            connect(nextBtn, &QPushButton::clicked, this, &MainWindow::onModuleButtonClicked);
        }
    }
}