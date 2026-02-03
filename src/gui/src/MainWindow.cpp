#include "MainWindow.hpp"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QSqlDatabase& database, QWidget *parent)
    : QMainWindow(parent)
    , database_(&database)
    , authWindow_(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground, false);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setupUI();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , database_(nullptr)
    , authWindow_(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground, false);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setupUI();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    centralWidget_ = std::make_unique<QWidget>(this);
    setCentralWidget(centralWidget_.get());

    setupMainLabel();
    setupMainButton();

    setupTitleBar();
    
    setupLayout();
    setupWindowProperties();
    setupConnections();
}

void MainWindow::onMainButtonClicked()
{
    qDebug() << "Main button is clicked";
    openAuthWindow();
}

void MainWindow::onCloseButtonClicked()
{
    qDebug() << "Close button is clicked";
    close();
}

void MainWindow::onMinimizeButtonClicked()
{
    qDebug() << "Minimize button is clicked";
    showMinimized();
}

void MainWindow::setupWindowProperties()
{
    setWindowState(Qt::WindowMaximized);
    setStyleSheet("background-color: #1d201a28");
    setCursor(Qt::ArrowCursor);
}

void MainWindow::setupFaviconLabel()
{
    faviconLabel_ = std::make_unique<QLabel>(titleBar_.get());
    qDebug() << "Setting favicon";
    faviconLabel_->setPixmap(QIcon("src/gui/resources/icons/icon.ico").pixmap(24, 24));
    faviconLabel_->setStyleSheet("background-color: transparent;");
}

void MainWindow::setupWindowTitleLabel()
{
    windowTitleLabel_ = std::make_unique<QLabel>("cppforge", titleBar_.get());
    windowTitleLabel_->setStyleSheet("color: white; font-size: 14px; background-color: transparent;");
}

void MainWindow::setupLabelFont(QLabel *label_)
{
    labelFont_ = std::make_unique<QFont>();
    labelFont_->setFamily("Cascadia");
    labelFont_->setPixelSize(48);
    labelFont_->setBold(true);
    labelFont_->setItalic(true);
    label_->setFont(*labelFont_);
    label_->setWordWrap(true);
    label_->setStyleSheet("color: white;");
}

void MainWindow::setupMainButtonFont()
{
    mainButtonFont_ = std::make_unique<QFont>();
    mainButtonFont_->setFamily("Cascadia");
    mainButtonFont_->setPixelSize(30);
    mainButtonFont_->setBold(true);
    mainButton_->setFont(*mainButtonFont_);
}

void MainWindow::setupMainLabel()
{
    mainLabel_ = std::make_unique<QLabel>("my code learning application");
    setupLabelFont(mainLabel_.get());
    mainLabel_->setAlignment(Qt::AlignCenter);
}

void MainWindow::setupMainButton()
{
    mainButton_ = std::make_unique<QPushButton>("Sign In");
    setupMainButtonFont();
    mainButton_->setCursor(Qt::PointingHandCursor);
    mainButton_->setFont(*mainButtonFont_);
    mainButton_->setStyleSheet("background-color: #b0b96aff; "
                               "color: white; "
                               "border: none; "
                               "padding: 10px;");
}

void MainWindow::setupCloseButton()
{
    closeButton_ = std::make_unique<QPushButton>("x", titleBar_.get());
    closeButton_->setFixedSize(30, 30);
    closeButton_->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: none;"
        "   font-size: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #aa3333;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #aa3333;"
        "}"
        "QPushButton:focus {"
        "   background-color: transparent;"
        "   outline: none;"
        "}"
        "QPushButton:default {"
        "   background-color: transparent;"
        "}");
    closeButton_->setCursor(Qt::PointingHandCursor);
}

void MainWindow::setupMinimizeButton()
{
    minimizeButton_ = std::make_unique<QPushButton>("_", titleBar_.get());
    minimizeButton_->setFixedSize(30, 30);
    minimizeButton_->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: none;"
        "   font-size: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #aa3333;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #aa3333;"
        "}"
        "QPushButton:focus {"
        "   background-color: transparent;"
        "   outline: none;"
        "}"
        "QPushButton:default {"
        "   background-color: transparent;"
        "}");
    minimizeButton_->setCursor(Qt::PointingHandCursor);
}

void MainWindow::setupTitleBar()
{
    titleBar_ = std::make_unique<QWidget>(this);
    titleBar_->setFixedHeight(40);
    titleBar_->setStyleSheet("background-color: #2d2d30;");

    auto titleBarLayout = std::make_unique<QHBoxLayout>(titleBar_.get());
    titleBarLayout->setContentsMargins(10, 0, 5, 0);
    titleBarLayout->setSpacing(10);

    setupFaviconLabel();
    setupWindowTitleLabel();
    
    setupMinimizeButton();
    setupCloseButton();

    titleBarLayout->addWidget(faviconLabel_.get());
    titleBarLayout->addWidget(windowTitleLabel_.get());

    titleBarLayout->addStretch();
    
    titleBarLayout->addWidget(minimizeButton_.get());
    titleBarLayout->addWidget(closeButton_.get());

    titleBar_->setLayout(titleBarLayout.release());
}

void MainWindow::setupLayout()
{
    mainLayout_ = std::make_unique<QVBoxLayout>(centralWidget_.get());

    mainLayout_->addWidget(titleBar_.get());

    mainLayout_->addWidget(mainLabel_.get());
    mainLayout_->addWidget(mainButton_.get());
    
    mainLayout_->setAlignment(mainLabel_.get(), Qt::AlignCenter);
    mainLayout_->setAlignment(mainButton_.get(), Qt::AlignCenter);

    
    mainLayout_->setContentsMargins(0, 0, 0, 50);
}

void MainWindow::openAuthWindow()
{
    if (!authWindow_)
    {
        qDebug() << "Creating AuthWindow";
        
        if (database_ && database_->isOpen())
        {
            // Создаем AuthWindow с передачей БД
            authWindow_ = std::make_unique<AuthWindow>(*database_, this);
            qDebug() << "AuthWindow created with database connection";
        }
        else
        {
            // Создаем AuthWindow без БД
            authWindow_ = std::make_unique<AuthWindow>(this);
            qDebug() << "AuthWindow created without database connection";
        }
        
        authWindow_->setWindowModality(Qt::ApplicationModal);
    }
    
    qDebug() << "Showing AuthWindow";
    authWindow_->show();
    authWindow_->raise();
    authWindow_->activateWindow();
}

void MainWindow::setupConnections()
{
    connect(mainButton_.get(), &QPushButton::clicked, this, &MainWindow::onMainButtonClicked);
    connect(closeButton_.get(), &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);
    connect(minimizeButton_.get(), &QPushButton::clicked, this, &MainWindow::onMinimizeButtonClicked);
}