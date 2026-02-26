#include "MainWindow.hpp"
#include "CustomTitleBar.hpp"

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
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
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
    setWindowOpacity(0.0);
    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(150);
    transitionAnimation_->setStartValue(0.0);
    transitionAnimation_->setEndValue(1.0);
    transitionAnimation_->start();
}

void MainWindow::setupWindowProperties()
{
    setFixedSize(1280, 900);
    setWindowTitle("CppForge - Main Menu");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("MainWindow");
    setStyleSheet("#MainWindow { background-color: white; border: 1px solid #cccccc; }");
}

void MainWindow::setupTitleBar()
{
    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setTitle("CppForge - Main Menu");
    customTitleBar_->setIcon(windowIcon());
}

void MainWindow::setupLeftPanel()
{
    sideBar = std::make_unique<QFrame>(this);
    sideBar->setObjectName("sideBar");
    sideBar->setFixedWidth(220);

    auto layout = std::make_unique<QVBoxLayout>(sideBar.get());
    layout->setContentsMargins(20, 30, 20, 30);
    layout->setSpacing(15);

    auto logoIcon = std::make_unique<QLabel>();
    logoIcon->setAlignment(Qt::AlignCenter);
    logoIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QPixmap logoPixmap(":/icons/main_logo.ico");
    
    if (!logoPixmap.isNull()) {
        logoPixmap = logoPixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        logoIcon->setPixmap(logoPixmap);
        logoIcon->setFixedSize(120, 120);
        qDebug() << "MainWindow: Logo loaded";
    } else {
        logoIcon->setText("CppForge");
        logoIcon->setStyleSheet("color: #62639b; font-size: 24px; font-weight: bold;");
        logoIcon->setFixedSize(120, 120);
        qDebug() << "MainWindow: Logo not found, using text fallback";
    }

    auto learnBtn = std::make_unique<QPushButton>("  ОБУЧЕНИЕ");
    auto ratingBtn = std::make_unique<QPushButton>("  РЕЙТИНГ");
    auto profileBtn = std::make_unique<QPushButton>("  ПРОФИЛЬ");

    QFont btnFont("Roboto", 14, QFont::Bold);
    for (auto btn : {learnBtn.get(), ratingBtn.get(), profileBtn.get()}) {
        btn->setFont(btnFont);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(50);
        btn->setStyleSheet("text-align: left; padding-left: 15px;");
    }

    layout->addWidget(logoIcon.release(), 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(learnBtn.release());
    layout->addWidget(ratingBtn.release());
    layout->addWidget(profileBtn.release());
    layout->addStretch();

    layout.release();
}

void MainWindow::setupCenterPanel()
{
    centerPanelLayout_ = std::make_unique<QVBoxLayout>();
    centerPanelLayout_->setContentsMargins(0, 0, 0, 0);
    centerPanelLayout_->setSpacing(20);

    eventCard = std::make_unique<QFrame>();
    eventCard->setProperty("class", QVariant("card"));
    auto eLayout = std::make_unique<QVBoxLayout>(eventCard.get());
    eLayout->setContentsMargins(25, 25, 25, 25);
    eLayout->setSpacing(15);
    
    auto eventTitle = std::make_unique<QLabel>("СОБЫТИЕ");
    eventTitle->setProperty("class", "section-title");
    QFont eventFont("Roboto", 20, QFont::Bold);
    eventTitle->setFont(eventFont);
    eLayout->addWidget(eventTitle.release());

    dailyTaskCard = std::make_unique<QFrame>();
    dailyTaskCard->setProperty("class", QVariant("card"));
    auto dLayout = std::make_unique<QVBoxLayout>(dailyTaskCard.get());
    dLayout->setContentsMargins(25, 25, 25, 25);
    dLayout->setSpacing(15);

    auto dailyTitle = std::make_unique<QLabel>("Задание дня");
    QFont dailyTitleFont("Roboto", 16, QFont::Bold);
    dailyTitle->setFont(dailyTitleFont);

    auto dailyDesc = std::make_unique<QLabel>("Выполнить 1 задание");
    QFont dailyFont("Roboto", 14);
    dailyDesc->setFont(dailyFont);

    auto dailyProgressText = std::make_unique<QLabel>("0/1");
    dailyProgressText->setFont(dailyFont);
    dailyProgressText->setAlignment(Qt::AlignRight);

    auto dailyProgress = std::make_unique<QProgressBar>();
    dailyProgress->setValue(0);
    dailyProgress->setFixedHeight(8);

    dLayout->addWidget(dailyTitle.release());
    dLayout->addWidget(dailyDesc.release());
    dLayout->addWidget(dailyProgressText.release());
    dLayout->addWidget(dailyProgress.release());

    auto footerWidget = std::make_unique<QWidget>();
    footerLinksLayout = std::make_unique<QHBoxLayout>(footerWidget.get());
    footerLinksLayout->setContentsMargins(0, 20, 0, 0);
    footerLinksLayout->setSpacing(20);
    
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

    centerPanelLayout_->addWidget(eventCard.get());
    centerPanelLayout_->addWidget(dailyTaskCard.get());
    centerPanelLayout_->addWidget(footerWidget.release());
    centerPanelLayout_->addStretch();

    eLayout.release();
    dLayout.release();
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

    QFont titleFont("Roboto", 16, QFont::Bold);
    QFont statusFont("Roboto", 12);
    QFont btnFont("Roboto", 12);

    moduleCards.clear();
    moduleProgressBars.clear();
    moduleProgressLabels.clear();
    moduleButtons.clear();

    for (int i = 1; i <= 14; ++i) {
        auto moduleCard = std::make_unique<QFrame>();
        moduleCard->setProperty("class", QVariant("card"));
        auto mLayout = std::make_unique<QVBoxLayout>(moduleCard.get());
        mLayout->setContentsMargins(20, 20, 20, 20);
        mLayout->setSpacing(10);

        auto mTitle = std::make_unique<QLabel>(QString("Module %1").arg(i));
        mTitle->setFont(titleFont);

        auto progress = std::make_unique<QProgressBar>();
        progress->setValue(0);
        progress->setFixedHeight(8);

        auto progressLabel = std::make_unique<QLabel>("0% выполнено");
        progressLabel->setFont(statusFont);
        progressLabel->setProperty("class", "progress-text");

        bool isLocked = (i != 1);
        
        auto button = std::make_unique<QPushButton>(isLocked ? "Заблокировано" : "Далее →");
        button->setObjectName("moduleBtn");
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedHeight(40);
        button->setFont(btnFont);
        
        if (isLocked) {
            button->setEnabled(false);
        }

        mLayout->addWidget(mTitle.release());
        mLayout->addWidget(progress.release());
        mLayout->addWidget(progressLabel.release());
        mLayout->addWidget(button.release());

        moduleProgressBars.append(progress.get());
        moduleProgressLabels.append(progressLabel.get());
        moduleButtons.append(button.get());
        
        modulesLayout->addWidget(moduleCard.get());
        moduleCards.push_back(std::move(moduleCard));

        mLayout.release();
    }

    modulesLayout->addStretch();
    modulesScrollArea->setWidget(modulesContainer.release());
}

void MainWindow::setupUI()
{
    setupWindowProperties();
    setupTitleBar();
    setupStyles();

    auto mainVerticalLayout = std::make_unique<QVBoxLayout>(this);
    mainVerticalLayout->setContentsMargins(0, 0, 0, 0);
    mainVerticalLayout->setSpacing(0);
    
    mainVerticalLayout->addWidget(customTitleBar_.get());

    auto contentContainer = std::make_unique<QWidget>();
    contentContainer->setObjectName("contentContainer");
    auto containerLayout = std::make_unique<QHBoxLayout>(contentContainer.get());
    containerLayout->setContentsMargins(30, 30, 30, 30);
    containerLayout->setSpacing(30);

    setupLeftPanel();                          
    setupRightPanel();                         
    setupCenterPanel();                        

    auto eventWidget = std::make_unique<QWidget>();
    eventWidget->setLayout(centerPanelLayout_.get());

    containerLayout->addWidget(sideBar.get(), 1);
    containerLayout->addWidget(modulesScrollArea.get(), 2);
    containerLayout->addWidget(eventWidget.release(), 1);

    mainVerticalLayout->addWidget(contentContainer.release());
    mainVerticalLayout.release();
}

void MainWindow::setupStyles()
{
    setStyleSheet(R"(
        QWidget {
            background-color: #f8f9fa;
            font-family: Roboto, Arial, sans-serif;
        }

        #MainWindow {
            background-color: white;
            border: 1px solid rgba(0, 0, 0, 0.1);
            border-radius: 20px;
        }

        QFrame#sideBar {
            background-color: white;
            border-radius: 20px;
            border: 1px solid rgba(0, 0, 0, 0.05);
        }

        QFrame[class="card"] {
            background-color: white;
            border-radius: 20px;
            border: 1px solid rgba(0, 0, 0, 0.05);
            padding: 0px;
        }

        QScrollArea#modulesScrollArea {
            background-color: transparent;
            border: none;
        }

        QScrollArea#modulesScrollArea > QWidget > QWidget {
            background-color: transparent;
        }

        QPushButton {
            background-color: #f0f0f0;
            border: none;
            border-radius: 12px;
            font-weight: 600;
            color: #333;
            text-align: left;
            padding-left: 15px;
            margin: 2px 0;
        }

        QPushButton:hover {
            background-color: #62639b;
            color: white;
        }

        QPushButton:pressed {
            background-color: #4B4C76;
            color: white;
        }

        QPushButton#moduleBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #62639b, stop:1 #7B7CB5);
            color: white;
            font-weight: bold;
            border-radius: 10px;
            padding: 10px;
            text-align: center;
            border: none;
        }

        QPushButton#moduleBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #7B7CB5, stop:1 #62639b);
            font-weight: bold;
        }

        QPushButton#moduleBtn:pressed {
            background: #4B4C76;
        }

        QPushButton#moduleBtn:disabled {
            background: #d0d0d0;
            color: #888;
        }

        QProgressBar {
            background: #e9ecef;
            border-radius: 4px;
            height: 8px;
            text-align: center;
        }

        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #62639b, stop:1 #7B7CB5);
            border-radius: 4px;
        }

        QLabel[class="progress-text"] {
            color: #666;
            font-size: 12px;
            font-weight: 500;
        }

        QLabel[class="section-title"] {
            color: #62639b;
            font-size: 20px;
            font-weight: bold;
            letter-spacing: 0.5px;
        }

        QScrollBar:vertical {
            background: transparent;
            width: 8px;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical {
            background: #c0c0c0;
            border-radius: 4px;
            min-height: 30px;
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

        QPushButton[class="footer-link"] {
            color: #666;
            font-size: 13px;
            font-weight: normal;
            background: transparent;
            border: none;
            padding: 5px 8px;
            text-align: left;
        }

        QPushButton[class="footer-link"]:hover {
            color: #62639b;
            font-weight: bold;
        }
    )");
}