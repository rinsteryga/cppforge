#include "MainWindow.hpp"
#include "CustomTitleBar.hpp"
#include "TaskWindow.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QVariant>
#include <QPushButton>
#include <QProgressBar>
#include <QSpacerItem>
#include <QScreen>
#include <QTimer>
#include <QPainter>
#include <QStyleOption>
#include <QFont>
#include <QDebug>
#include <QGuiApplication>
#include <QIcon>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , isTransitioning_(false)
    , pendingModuleId_(-1)
{
    setupUI();
    setWindowOpacity(0.0);
    QTimer::singleShot(50, this, &MainWindow::centerWindow);
    QTimer::singleShot(100, this, &MainWindow::fadeIn);
}

MainWindow::~MainWindow() = default;

void MainWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!isTransitioning_) {
        setWindowOpacity(0.0);
        fadeIn();
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
                connect(taskWindow_.get(), &TaskWindow::moduleProgressUpdated,
                        this, &MainWindow::updateModuleProgress);
                connect(taskWindow_.get(), &TaskWindow::windowClosed,
                        this, &MainWindow::onTaskWindowClosed);
            }
            
            taskWindow_->loadModule(pendingModuleId_);
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
    setAttribute(Qt::WA_TranslucentBackground);
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

    auto shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 30));
    shadowEffect->setOffset(0, 4);
    sideBar->setGraphicsEffect(shadowEffect);

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
    
    if (!logoPixmap.isNull()) {
        logoPixmap = logoPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        logoIcon->setPixmap(logoPixmap);
        logoIcon->setFixedSize(100, 100);
        qDebug() << "MainWindow: Logo loaded";
    } else {
        logoIcon->setText("CppForge");
        logoIcon->setStyleSheet("color: #62639b; font-size: 24px; font-weight: bold;");
        logoIcon->setFixedSize(100, 100);
        qDebug() << "MainWindow: Logo not found, using text fallback";
    }
    
    logoLayout->addWidget(logoIcon.release());
    logoContainer->setLayout(logoLayout.release());
    
    learnBtn = new QPushButton("Учиться");
    auto ratingBtn = std::make_unique<QPushButton>("Рейтинг");
    auto profileBtn = std::make_unique<QPushButton>("Профиль");

    QFont btnFont("Roboto", 13, QFont::Medium);
    for (auto btn : {learnBtn, ratingBtn.get(), profileBtn.get()}) {
        btn->setFont(btnFont);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(48);
        btn->setObjectName("navButton");
    }

    connect(learnBtn, &QPushButton::clicked, this, &MainWindow::onLearnButtonClicked);

    layout->addWidget(logoContainer.release(), 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(learnBtn);
    layout->addWidget(ratingBtn.release());
    layout->addWidget(profileBtn.release());
    layout->addStretch();
    
    sideBar->setLayout(layout.release());
}

void MainWindow::setupCenterPanel()
{
    centerPanelLayout_ = std::make_unique<QVBoxLayout>();
    centerPanelLayout_->setContentsMargins(0, 0, 0, 0);
    centerPanelLayout_->setSpacing(20);
    centerPanelLayout_->setAlignment(Qt::AlignTop);

    // Карточка событий
    eventCard = std::make_unique<QFrame>();
    eventCard->setProperty("class", QVariant("card"));
    eventCard->setObjectName("eventCard");
    
    auto shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 15));
    shadowEffect->setOffset(0, 4);
    eventCard->setGraphicsEffect(shadowEffect);
    
    auto eLayout = std::make_unique<QVBoxLayout>();
    eLayout->setContentsMargins(25, 25, 25, 25);
    eLayout->setSpacing(15);
    
    auto eventTitle = std::make_unique<QLabel>("События");
    eventTitle->setProperty("class", "section-title");
    QFont eventFont("Roboto", 18, QFont::Bold);
    eventTitle->setFont(eventFont);
    eventTitle->setStyleSheet("color: #333;");
    
    auto eventPlaceholder = std::make_unique<QLabel>("Нет предстоящих событий");
    eventPlaceholder->setObjectName("eventPlaceholder");
    eventPlaceholder->setAlignment(Qt::AlignCenter);
    eventPlaceholder->setFixedHeight(80);
    
    eLayout->addWidget(eventTitle.release());
    eLayout->addWidget(eventPlaceholder.release());
    eLayout->addStretch();
    eventCard->setLayout(eLayout.release());

    // Карточка задания дня
    dailyTaskCard = std::make_unique<QFrame>();
    dailyTaskCard->setProperty("class", QVariant("card"));
    dailyTaskCard->setObjectName("dailyTaskCard");
    
    auto shadowEffect2 = new QGraphicsDropShadowEffect();
    shadowEffect2->setBlurRadius(20);
    shadowEffect2->setColor(QColor(0, 0, 0, 15));
    shadowEffect2->setOffset(0, 4);
    dailyTaskCard->setGraphicsEffect(shadowEffect2);
    
    auto dLayout = std::make_unique<QVBoxLayout>();
    dLayout->setContentsMargins(25, 25, 25, 25);
    dLayout->setSpacing(15);

    auto dailyHeader = std::make_unique<QHBoxLayout>();
    
    auto dailyTitle = std::make_unique<QLabel>("Задание дня");
    QFont dailyTitleFont("Roboto", 18, QFont::Bold);
    dailyTitle->setFont(dailyTitleFont);
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
    QFont dailyFont("Roboto", 14);
    dailyDesc->setFont(dailyFont);
    dailyDesc->setStyleSheet("color: #555; font-weight: 500;");

    auto dailyProgress = std::make_unique<QProgressBar>();
    dailyProgress->setValue(0);
    dailyProgress->setFixedHeight(16);
    dailyProgress->setObjectName("dailyProgress");

    dLayout->addLayout(dailyHeader.release());
    dLayout->addWidget(dailyDesc.release());
    dLayout->addWidget(dailyProgress.release());
    dailyTaskCard->setLayout(dLayout.release());

    // Футер с ссылками
    auto footerWidget = std::make_unique<QWidget>();
    footerWidget->setObjectName("footerWidget");
    footerLinksLayout = std::make_unique<QHBoxLayout>(footerWidget.get());
    footerLinksLayout->setContentsMargins(5, 20, 5, 0);
    footerLinksLayout->setSpacing(15);
    
    auto createLink = [](const QString& text) {
        auto btn = std::make_unique<QPushButton>(text);
        btn->setProperty("class", "footer-link");
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };
    
    auto aboutBtnPtr = createLink("О CppForge");
    auto contactsBtnPtr = createLink("Контакты");
    auto privacyBtnPtr = createLink("Конфиденциальность");
    
    aboutBtn = aboutBtnPtr.get();
    contactsBtn = contactsBtnPtr.get();
    privacyBtn = privacyBtnPtr.get();
    
    footerLinksLayout->addWidget(aboutBtnPtr.release());
    footerLinksLayout->addWidget(contactsBtnPtr.release());
    footerLinksLayout->addWidget(privacyBtnPtr.release());
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
    modulesScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    modulesScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    modulesContainer = std::make_unique<QWidget>();
    modulesLayout = std::make_unique<QVBoxLayout>(modulesContainer.get());
    modulesLayout->setContentsMargins(0, 0, 0, 0);
    modulesLayout->setSpacing(15);

    QFont titleFont("Roboto", 15, QFont::Bold);
    QFont statusFont("Roboto", 12);
    QFont btnFont("Roboto", 12, QFont::Medium);

    moduleCards.clear();
    moduleProgressBars.clear();
    moduleProgressLabels.clear();
    moduleButtons.clear();

    for (int i = 1; i <= 14; ++i) {
        auto moduleCard = std::make_unique<QFrame>();
        moduleCard->setProperty("class", QVariant("card"));
        moduleCard->setObjectName(QString("moduleCard_%1").arg(i));
        
        auto shadowEffect = new QGraphicsDropShadowEffect();
        shadowEffect->setBlurRadius(15);
        shadowEffect->setColor(QColor(0, 0, 0, 10));
        shadowEffect->setOffset(0, 2);
        moduleCard->setGraphicsEffect(shadowEffect);
        
        auto mLayout = std::make_unique<QVBoxLayout>();
        mLayout->setContentsMargins(20, 20, 20, 20);
        mLayout->setSpacing(12);

        auto mTitle = std::make_unique<QLabel>(QString("Модуль %1").arg(i));
        mTitle->setFont(titleFont);
        mTitle->setStyleSheet("color: #333;");

        auto progress = std::make_unique<QProgressBar>();
        progress->setValue(0);
        progress->setFixedHeight(16);
        progress->setObjectName("moduleProgress");

        auto progressLabel = std::make_unique<QLabel>("0% выполнено");
        progressLabel->setFont(statusFont);
        progressLabel->setProperty("class", "progress-text");

        bool isLocked = (i != 1);
        
        auto button = std::make_unique<QPushButton>(isLocked ? "Заблокировано" : "Начать обучение");
        button->setObjectName("moduleBtn");
        button->setProperty("moduleId", i);
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedHeight(42);
        button->setFont(btnFont);
        
        if (isLocked) {
            button->setEnabled(false);
            button->setStyleSheet("background: #f0f0f0; color: #999;");
        } else {
            connect(button.get(), &QPushButton::clicked, this, &MainWindow::onModuleButtonClicked);
        }

        mLayout->addWidget(mTitle.release());
        mLayout->addWidget(progress.release());
        mLayout->addWidget(progressLabel.release());
        mLayout->addWidget(button.release(), 0, Qt::AlignRight);
        
        moduleCard->setLayout(mLayout.release());

        moduleProgressBars.append(progress.get());
        moduleProgressLabels.append(progressLabel.get());
        moduleButtons.append(button.get());
        
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

    auto contentContainer = std::make_unique<QWidget>();
    contentContainer->setObjectName("contentContainer");
    auto containerLayout = std::make_unique<QHBoxLayout>();
    containerLayout->setContentsMargins(30, 30, 30, 30);
    containerLayout->setSpacing(30);

    setupLeftPanel();                          
    setupRightPanel();                         
    setupCenterPanel();                        

    auto eventWidget = std::make_unique<QWidget>();
    eventWidget->setLayout(centerPanelLayout_.release());

    containerLayout->addWidget(sideBar.get(), 1);
    containerLayout->addWidget(modulesScrollArea.get(), 2);
    containerLayout->addWidget(eventWidget.release(), 1);
    contentContainer->setLayout(containerLayout.release());

    mainVerticalLayout->addWidget(contentContainer.release());
    setLayout(mainVerticalLayout.release());
    
    setupConnections();
}

void MainWindow::setupConnections()
{
    // Дополнительные подключения при необходимости
}

void MainWindow::setupStyles()
{
    setStyleSheet(R"(
        QWidget {
            background-color: #f5f7fb;
            font-family: 'Roboto', 'Segoe UI', Arial, sans-serif;
        }

        #MainWindow {
            background-color: white;
            border: none;
            border-radius: 24px;
        }

        QFrame#sideBar {
            background-color: white;
            border-radius: 24px;
            border: none;
        }

        QFrame#logoContainer {
            background-color: #f8f9ff;
            border-radius: 20px;
            padding: 15px;
        }

        QPushButton#navButton {
            background-color: transparent;
            border: none;
            border-radius: 12px;
            font-weight: 500;
            color: #555;
            text-align: left;
            padding-left: 20px;
            margin: 2px 0;
        }

        QPushButton#navButton:hover {
            background-color: #f0f2ff;
            color: #62639b;
        }

        QPushButton#navButton:pressed {
            background-color: #e6e8ff;
            color: #4B4C76;
        }

        QFrame[class="card"] {
            background-color: white;
            border-radius: 24px;
            border: none;
        }

        QFrame#eventCard {
            min-height: 150px;
        }

        QLabel#eventPlaceholder {
            color: #aaa;
            font-size: 14px;
            background-color: #f8f9fc;
            border-radius: 16px;
            padding: 10px;
        }

        QFrame#dailyTaskCard {
            min-height: 150px;
        }

        QLabel#dailyDesc {
            color: #555;
            font-size: 15px;
            font-weight: 500;
            margin-top: 5px;
            margin-bottom: 5px;
        }

        QLabel#dailyProgressText {
            color: #333;
            font-weight: 700;
            font-size: 16px;
        }

        QProgressBar#dailyProgress {
            background: #eef0f5;
            border-radius: 8px;
            height: 16px;
            margin-top: 5px;
            text-align: center;
            font-size: 11px;
            color: #333;
            font-weight: 500;
        }

        QProgressBar#dailyProgress::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #62639b, stop:1 #7B7CB5);
            border-radius: 8px;
        }

        QScrollArea#modulesScrollArea {
            background-color: transparent;
            border: none;
        }

        QScrollArea#modulesScrollArea > QWidget > QWidget {
            background-color: transparent;
        }

        QPushButton#moduleBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #62639b, stop:1 #7B7CB5);
            color: white;
            font-weight: 600;
            border-radius: 10px;
            padding: 8px 20px;
            text-align: center;
            border: none;
            min-width: 130px;
        }

        QPushButton#moduleBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #7B7CB5, stop:1 #62639b);
        }

        QPushButton#moduleBtn:pressed {
            background: #4B4C76;
        }

        QPushButton#moduleBtn:disabled {
            background: #f0f0f0;
            color: #999;
            border: 1px solid #e0e0e0;
        }

        QProgressBar#moduleProgress {
            background: #eef0f5;
            border-radius: 8px;
            height: 16px;
            text-align: center;
            font-size: 11px;
            color: #333;
            font-weight: 500;
        }

        QProgressBar#moduleProgress::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #62639b, stop:1 #7B7CB5);
            border-radius: 8px;
        }

        QLabel[class="progress-text"] {
            color: #777;
            font-size: 12px;
            font-weight: 500;
            margin-top: 6px;
        }

        QLabel[class="section-title"] {
            color: #333;
            font-size: 18px;
            font-weight: bold;
            letter-spacing: 0.3px;
        }

        QScrollBar:vertical {
            background: transparent;
            width: 6px;
            border-radius: 3px;
        }

        QScrollBar::handle:vertical {
            background: #d0d0d0;
            border-radius: 3px;
            min-height: 40px;
        }

        QScrollBar::handle:vertical:hover {
            background: #a0a0a0;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
        }

        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
        }

        QWidget#footerWidget {
            background-color: transparent;
        }

        QPushButton[class="footer-link"] {
            color: #999;
            font-size: 13px;
            font-weight: normal;
            background: transparent;
            border: none;
            padding: 6px 12px;
            text-align: left;
            border-radius: 6px;
        }

        QPushButton[class="footer-link"]:hover {
            color: #62639b;
            background-color: #f0f2ff;
        }

        QFrame[class="separator"] {
            background-color: #eef0f5;
            max-height: 1px;
            min-height: 1px;
            margin: 10px 0;
        }
    )");
}

void MainWindow::onModuleButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int moduleId = button->property("moduleId").toInt();
        qDebug() << "Module button clicked:" << moduleId;
        
        // Убедимся, что модуль существует в JSON
        if (moduleId == 1) {
            animateToTaskWindow(moduleId);
        } else {
            qDebug() << "Module" << moduleId << "not available yet";
            // Можно показать сообщение, что модуль заблокирован
        }
    }
}

void MainWindow::onLearnButtonClicked()
{
    qDebug() << "Learn button clicked";
    animateToTaskWindow(0);
}

void MainWindow::animateToTaskWindow(int moduleId)
{
    if (isTransitioning_) {
        pendingModuleId_ = moduleId;
        return;
    }
    
    isTransitioning_ = true;
    pendingModuleId_ = moduleId;
    fadeOut();
}

void MainWindow::openTaskWindow(int moduleId)
{
    if (!taskWindow_) {
        taskWindow_ = std::make_unique<TaskWindow>();
        connect(taskWindow_.get(), &TaskWindow::moduleProgressUpdated,
                this, &MainWindow::updateModuleProgress);
        connect(taskWindow_.get(), &TaskWindow::windowClosed,
                this, &MainWindow::onTaskWindowClosed);
    }
    
    taskWindow_->loadModule(moduleId);
    taskWindow_->show();
    taskWindow_->fadeIn();
}

void MainWindow::onTaskWindowClosed()
{
    // Плавно показываем главное окно при закрытии окна заданий
    show();
    fadeIn();
}

void MainWindow::updateModuleProgress(int moduleId, int progress)
{
    if (moduleId >= 1 && moduleId <= moduleProgressBars.size()) {
        moduleProgressBars[moduleId - 1]->setValue(progress);
        moduleProgressLabels[moduleId - 1]->setText(QString("%1% выполнено").arg(progress));
    }
    
    if (progress == 100 && moduleId < moduleButtons.size()) {
        QPushButton* nextButton = moduleButtons[moduleId];
        if (nextButton && !nextButton->isEnabled()) {
            nextButton->setEnabled(true);
            nextButton->setText("Начать обучение");
            nextButton->setStyleSheet("");
            connect(nextButton, &QPushButton::clicked, this, &MainWindow::onModuleButtonClicked);
        }
    }
}