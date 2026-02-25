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

    QVBoxLayout* layout = new QVBoxLayout(sideBar.get());
    layout->setContentsMargins(20, 30, 20, 30);
    layout->setSpacing(15);

    QLabel* logo = new QLabel("CppForge");
    QFont logoFont("Roboto", 28, QFont::Bold);
    logo->setFont(logoFont);
    logo->setStyleSheet("color: #62639b; padding: 10px 0;");

    QPushButton* learnBtn = new QPushButton("  ОБУЧЕНИЕ");
    QPushButton* ratingBtn = new QPushButton("  РЕЙТИНГ");
    QPushButton* profileBtn = new QPushButton("  ПРОФИЛЬ");

    QFont btnFont("Roboto", 14, QFont::Bold);
    for (auto btn : {learnBtn, ratingBtn, profileBtn}) {
        btn->setFont(btnFont);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(50);
        btn->setStyleSheet("text-align: left; padding-left: 15px;");
    }

    layout->addWidget(logo);
    layout->addSpacing(20);
    layout->addWidget(learnBtn);
    layout->addWidget(ratingBtn);
    layout->addWidget(profileBtn);
    layout->addStretch();
}

void MainWindow::setupCenterPanel()  // СПРАВА - события
{
    centerPanelLayout_ = std::make_unique<QVBoxLayout>();
    centerPanelLayout_->setContentsMargins(0, 0, 0, 0);
    centerPanelLayout_->setSpacing(20);

    // Карточка события
    eventCard = std::make_unique<QFrame>();
    eventCard->setProperty("class", QVariant("card"));
    QVBoxLayout* eLayout = new QVBoxLayout(eventCard.get());
    eLayout->setContentsMargins(25, 25, 25, 25);
    eLayout->setSpacing(15);
    
    QLabel* eventTitle = new QLabel("СОБЫТИЕ");
    QFont eventFont("Roboto", 20, QFont::Bold);
    eventTitle->setFont(eventFont);
    eventTitle->setStyleSheet("color: #62639b;");
    eLayout->addWidget(eventTitle);

    // Карточка задания дня
    dailyTaskCard = std::make_unique<QFrame>();
    dailyTaskCard->setProperty("class", QVariant("card"));
    QVBoxLayout* dLayout = new QVBoxLayout(dailyTaskCard.get());
    dLayout->setContentsMargins(25, 25, 25, 25);
    dLayout->setSpacing(15);

    QLabel* dailyTitle = new QLabel("Задание дня");
    QFont dailyTitleFont("Roboto", 16, QFont::Bold);
    dailyTitle->setFont(dailyTitleFont);

    QLabel* dailyDesc = new QLabel("Выполнить 1 задание");
    QFont dailyFont("Roboto", 14);
    dailyDesc->setFont(dailyFont);

    QLabel* dailyProgressText = new QLabel("0/1");
    dailyProgressText->setFont(dailyFont);
    dailyProgressText->setAlignment(Qt::AlignRight);

    QProgressBar* dailyProgress = new QProgressBar;
    dailyProgress->setValue(0);
    dailyProgress->setFixedHeight(12);

    dLayout->addWidget(dailyTitle);
    dLayout->addWidget(dailyDesc);
    dLayout->addWidget(dailyProgressText);
    dLayout->addWidget(dailyProgress);

    centerPanelLayout_->addWidget(eventCard.get());
    centerPanelLayout_->addWidget(dailyTaskCard.get());
    centerPanelLayout_->addStretch();
}

void MainWindow::setupRightPanel()  // ЦЕНТР - модули с прокруткой
{
    // Создаем скролл-область
    modulesScrollArea = std::make_unique<QScrollArea>();
    modulesScrollArea->setObjectName("modulesScrollArea");
    modulesScrollArea->setWidgetResizable(true);
    modulesScrollArea->setFrameStyle(QFrame::NoFrame);
    modulesScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    modulesScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Контейнер для модулей
    modulesContainer = std::make_unique<QWidget>();
    modulesLayout = std::make_unique<QVBoxLayout>(modulesContainer.get());
    modulesLayout->setContentsMargins(0, 0, 0, 0);
    modulesLayout->setSpacing(15);

    QFont titleFont("Roboto", 16, QFont::Bold);
    QFont statusFont("Roboto", 12);
    QFont btnFont("Roboto", 12);

    // Очищаем списки перед заполнением
    moduleCards.clear();
    moduleProgressBars.clear();
    moduleProgressLabels.clear();
    moduleButtons.clear();

    // Создаем 14 модулей
    for (int i = 1; i <= 14; ++i) {
        auto moduleCard = std::make_unique<QFrame>();
        moduleCard->setProperty("class", QVariant("card"));
        QVBoxLayout* mLayout = new QVBoxLayout(moduleCard.get());
        mLayout->setContentsMargins(20, 20, 20, 20);
        mLayout->setSpacing(10);

        // Название модуля
        QLabel* mTitle = new QLabel(QString("Module %1").arg(i));
        mTitle->setFont(titleFont);

        // Прогресс-бар
        QProgressBar* progress = new QProgressBar;
        progress->setValue(0);
        progress->setFixedHeight(10);

        // Текст прогресса
        QLabel* progressLabel = new QLabel("0% выполнено");
        progressLabel->setFont(statusFont);
        progressLabel->setStyleSheet("color: #666;");

        // Кнопка модуля
        bool isLocked = (i != 1);
        
        QPushButton* button = new QPushButton(isLocked ? "Заблокировано" : "Далее →");
        button->setObjectName("moduleBtn");
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedHeight(40);
        button->setFont(btnFont);
        
        if (isLocked) {
            button->setEnabled(false);
            button->setStyleSheet("background-color: #cccccc; color: #666;");
        }

        // Добавляем всё в карточку
        mLayout->addWidget(mTitle);
        mLayout->addWidget(progress);
        mLayout->addWidget(progressLabel);
        mLayout->addWidget(button);

        // Сохраняем указатели на дочерние элементы (они будут удалены вместе с moduleCard)
        moduleProgressBars.append(progress);
        moduleProgressLabels.append(progressLabel);
        moduleButtons.append(button);
        
        // Сохраняем карточку в список умных указателей
        modulesLayout->addWidget(moduleCard.get());
        moduleCards.push_back(std::move(moduleCard));
    }

    modulesLayout->addStretch();
    modulesScrollArea->setWidget(modulesContainer.get());
}

void MainWindow::setupUI()
{
    setupWindowProperties();
    setupTitleBar();
    setupStyles();

    QVBoxLayout* mainVerticalLayout = new QVBoxLayout(this);
    mainVerticalLayout->setContentsMargins(0, 0, 0, 0);
    mainVerticalLayout->setSpacing(0);
    
    mainVerticalLayout->addWidget(customTitleBar_.get());

    QWidget* contentContainer = new QWidget();
    contentContainer->setObjectName("contentContainer");
    QHBoxLayout* containerLayout = new QHBoxLayout(contentContainer);
    containerLayout->setContentsMargins(30, 30, 30, 30);
    containerLayout->setSpacing(30);

    // Создаем панели
    setupLeftPanel();
    setupRightPanel(); // центр - модули
    setupCenterPanel(); // справа - события

    // Оборачиваем события в виджет
    QWidget* eventWidget = new QWidget();
    eventWidget->setLayout(centerPanelLayout_.get());

    // Располагаем: навигация | модули | события
    containerLayout->addWidget(sideBar.get(), 1);
    containerLayout->addWidget(modulesScrollArea.get(), 2);
    containerLayout->addWidget(eventWidget, 1);

    mainVerticalLayout->addWidget(contentContainer);
}

void MainWindow::setupStyles()
{
    setStyleSheet(R"(
        QWidget {
            background-color: #f8f9fa;
            font-family: Roboto, Arial, sans-serif;
        }

        QFrame#sideBar {
            background-color: white;
            border-radius: 20px;
            border: 1px solid #e0e0e0;
        }

        QFrame[class="card"] {
            background-color: white;
            border-radius: 20px;
            border: 1px solid #e0e0e0;
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
            border-radius: 10px;
            font-weight: bold;
            color: #333;
        }

        QPushButton:hover {
            background-color: #e5e5e5;
        }

        QPushButton:pressed {
            background-color: #d5d5d5;
        }

        QPushButton#moduleBtn {
            background-color: #62639b;
            color: white;
            font-weight: bold;
        }

        QPushButton#moduleBtn:hover {
            background-color: #7677B3;
        }

        QPushButton#moduleBtn:disabled {
            background-color: #cccccc;
            color: #666;
        }

        QProgressBar {
            background: #e9ecef;
            border-radius: 5px;
            height: 10px;
            text-align: center;
        }

        QProgressBar::chunk {
            background-color: #62639b;
            border-radius: 5px;
        }

        QScrollBar:vertical {
            background: #f0f0f0;
            width: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:vertical {
            background: #c0c0c0;
            border-radius: 6px;
            min-height: 30px;
        }

        QScrollBar::handle:vertical:hover {
            background: #a0a0a0;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
        }
    )");
}