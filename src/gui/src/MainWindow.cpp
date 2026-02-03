#include "MainWindow.hpp"
#include <QApplication>
#include <QScreen>
#include <QDebug>

// Конструктор с БД
MainWindow::MainWindow(QSqlDatabase& database, QWidget *parent)
    : QMainWindow(parent)
    , database_(&database)
{
    setupUI();
}

// Конструктор без БД
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , database_(nullptr)
{
    setupUI();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    setupWindowProperties();
    
    centralWidget_ = std::make_unique<QWidget>();
    setCentralWidget(centralWidget_.get());
    
    mainLayout_ = std::make_unique<QVBoxLayout>(centralWidget_.get());
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);
    
    setupTitleBar();
    setupMainLabel();
    setupMainButton();
    setupConnections();
}

void MainWindow::onMainButtonClicked()
{
    openAuthWindow();
}

void MainWindow::openAuthWindow()
{
    qDebug() << "Opening AuthWindow...";
    
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
    
    authWindow_->show();
}

void MainWindow::onCloseButtonClicked()
{
    qApp->quit();
}

void MainWindow::onMinimizeButtonClicked()
{
    showMinimized();
}

void MainWindow::setupWindowProperties()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(400, 500);
}

void MainWindow::setupFaviconLabel()
{
    faviconLabel_ = std::make_unique<QLabel>();
}

void MainWindow::setupWindowTitleLabel()
{
    windowTitleLabel_ = std::make_unique<QLabel>("CppForge");
}

void MainWindow::setupLabelFont(QLabel *label)
{
    if (!labelFont_)
    {
        labelFont_ = std::make_unique<QFont>("Arial", 16, QFont::Normal);
    }
    label->setFont(*labelFont_);
}

void MainWindow::setupMainButtonFont()
{
    if (!mainButtonFont_)
    {
        mainButtonFont_ = std::make_unique<QFont>("Arial", 12, QFont::Bold);
    }
}

void MainWindow::setupMainLabel()
{
    mainLabel_ = std::make_unique<QLabel>("Добро пожаловать в CppForge!");
    mainLabel_->setAlignment(Qt::AlignCenter);
    mainLayout_->addWidget(mainLabel_.get());
}

void MainWindow::setupMainButton()
{
    mainButton_ = std::make_unique<QPushButton>("Войти");
    setupMainButtonFont();
    mainButton_->setFont(*mainButtonFont_);
    mainButton_->setFixedSize(200, 50);
    mainLayout_->addWidget(mainButton_.get(), 0, Qt::AlignCenter);
}

void MainWindow::setupCloseButton()
{
    closeButton_ = std::make_unique<QPushButton>("✕");
    closeButton_->setFixedSize(30, 30);
}

void MainWindow::setupMinimizeButton()
{
    minimizeButton_ = std::make_unique<QPushButton>("－");
    minimizeButton_->setFixedSize(30, 30);
}

void MainWindow::setupTitleBar()
{
    titleBar_ = std::make_unique<QWidget>();
    titleBar_->setFixedHeight(40);
    
    QHBoxLayout *titleBarLayout = new QHBoxLayout(titleBar_.get());
    titleBarLayout->setContentsMargins(10, 0, 10, 0);
    
    setupFaviconLabel();
    setupWindowTitleLabel();
    setupCloseButton();
    setupMinimizeButton();
    
    titleBarLayout->addWidget(faviconLabel_.get());
    titleBarLayout->addWidget(windowTitleLabel_.get());
    titleBarLayout->addStretch();
    titleBarLayout->addWidget(minimizeButton_.get());
    titleBarLayout->addWidget(closeButton_.get());
    
    mainLayout_->addWidget(titleBar_.get());
}

void MainWindow::setupLayout()
{
    // Уже реализовано в setupUI()
}

void MainWindow::setupConnections()
{
    connect(mainButton_.get(), &QPushButton::clicked, this, &MainWindow::onMainButtonClicked);
    connect(closeButton_.get(), &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);
    connect(minimizeButton_.get(), &QPushButton::clicked, this, &MainWindow::onMinimizeButtonClicked);
}