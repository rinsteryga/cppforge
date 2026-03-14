#include "TaskWindow.hpp"
#include "CustomTitleBar.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QTextEdit>
#include <QScrollArea>
#include <QFrame>
#include <QScreen>
#include <QGuiApplication>
#include <QTimer>
#include <QPainter>
#include <QStyleOption>
#include <QIcon>

TaskWindow::TaskWindow(QWidget *parent) : QWidget(parent)
{
    setupUI();
    setWindowOpacity(0.0);
    QTimer::singleShot(50, this, &TaskWindow::centerWindow);
}

TaskWindow::~TaskWindow() = default;

void TaskWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void TaskWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    fadeIn();
}

void TaskWindow::centerWindow()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect adj = screen->availableGeometry();
        move(adj.x() + (adj.width() - width()) / 2, adj.y() + (adj.height() - height()) / 2);
    }
}

void TaskWindow::fadeIn()
{
    if (transitionAnimation_ && transitionAnimation_->state() == QPropertyAnimation::Running)
        transitionAnimation_->stop();

    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(300);
    transitionAnimation_->setStartValue(0.0);
    transitionAnimation_->setEndValue(1.0);
    transitionAnimation_->setEasingCurve(QEasingCurve::InOutCubic);
    transitionAnimation_->start();
}

void TaskWindow::fadeOut()
{
    if (transitionAnimation_ && transitionAnimation_->state() == QPropertyAnimation::Running)
        transitionAnimation_->stop();

    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(200);
    transitionAnimation_->setStartValue(1.0);
    transitionAnimation_->setEndValue(0.0);
    connect(transitionAnimation_.get(), &QPropertyAnimation::finished, this, [this]() { 
        hide(); 
        emit windowClosed(); 
    });
    transitionAnimation_->start();
}

void TaskWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setFixedSize(1200, 850);
    setObjectName("TaskWindow");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));

    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 1. Тайтлбар
    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setTitle("cppforge - Редактор заданий");
    customTitleBar_->setIcon(windowIcon());
    rootLayout->addWidget(customTitleBar_.get());

    // 2. Основная область со сплиттером
    auto mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(1);

    // --- ЛЕВАЯ ЧАСТЬ (Теория/Практика) ---
    auto leftContainer = new QWidget();
    auto leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // Заголовки вкладок
    auto tabHeader = new QWidget();
    tabHeader->setObjectName("tabHeader");
    tabHeader->setFixedHeight(45);
    auto tabLayout = new QHBoxLayout(tabHeader);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->setSpacing(0);

    auto btnPractice = new QPushButton("Практика");
    auto btnTheory = new QPushButton("Теория");
    btnPractice->setObjectName("tabButton");
    btnTheory->setObjectName("tabButton");
    btnPractice->setCheckable(true);
    btnTheory->setCheckable(true);
    btnPractice->setChecked(true);

    tabLayout->addWidget(btnPractice);
    tabLayout->addWidget(btnTheory);
    tabLayout->addStretch();
    leftLayout->addWidget(tabHeader);

    auto contentStack = new QStackedWidget();

    // Вкладка Теории
    auto theoryScroll = new QScrollArea();
    auto theoryContent = new QLabel("<h2>ТЕКТ ТЕОРИИ</h2><p>Инструкции и описание...</p>");
    theoryContent->setWordWrap(true);
    theoryContent->setContentsMargins(25, 25, 25, 25);
    theoryContent->setAlignment(Qt::AlignTop);
    theoryScroll->setWidget(theoryContent);
    theoryScroll->setWidgetResizable(true);

    // Вкладка Практики
    auto practiceScroll = new QScrollArea();
    auto practiceContent = new QLabel("<h2>ТЕКСТ ПРАКТИКИ</h2><p>Задание: напишите код...</p>");
    practiceContent->setWordWrap(true);
    practiceContent->setContentsMargins(25, 25, 25, 25);
    practiceContent->setAlignment(Qt::AlignTop);
    practiceScroll->setWidget(practiceContent);
    practiceScroll->setWidgetResizable(true);

    contentStack->addWidget(practiceScroll); // Index 0
    contentStack->addWidget(theoryScroll);   // Index 1
    leftLayout->addWidget(contentStack);

    // Кнопка Назад (как на фото в левом нижнем углу)
    auto footerLeft = new QHBoxLayout();
    footerLeft->setContentsMargins(10, 10, 10, 10);
    auto btnBack = new QPushButton();
    btnBack->setFixedSize(40, 40);
    btnBack->setObjectName("backButton");
    btnBack->setIcon(QIcon(":/icons/back_arrow.png")); // Если нет иконки, будет просто круг
    footerLeft->addWidget(btnBack);
    footerLeft->addStretch();
    leftLayout->addLayout(footerLeft);

    // --- ПРАВАЯ ЧАСТЬ (Код и Тесты) ---
    auto rightContainer = new QWidget();
    auto rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(10, 10, 10, 10);
    rightLayout->setSpacing(10);

    // Редактор кода
    auto codeFrame = new QFrame();
    codeFrame->setObjectName("editorFrame");
    auto codeLayout = new QVBoxLayout(codeFrame);
    
    auto codeTitle = new QLabel("<font color='#555'>&lt;/&gt; Code</font>");
    auto codeEditor = new QTextEdit();
    codeEditor->setObjectName("codeEditor");
    codeEditor->setPlaceholderText("// Твой код здесь...");

    auto codeActions = new QHBoxLayout();
    auto btnRun = new QPushButton("Run");
    auto btnSubmit = new QPushButton("Submit");
    btnRun->setObjectName("runButton");
    btnSubmit->setObjectName("submitButton");
    codeActions->addStretch();
    codeActions->addWidget(btnRun);
    codeActions->addWidget(btnSubmit);

    codeLayout->addWidget(codeTitle);
    codeLayout->addWidget(codeEditor);
    codeLayout->addLayout(codeActions);

    // Окно тестов
    auto testFrame = new QFrame();
    testFrame->setObjectName("testFrame");
    auto testLayout = new QVBoxLayout(testFrame);
    
    auto testTabs = new QHBoxLayout();
    testTabs->addWidget(new QLabel("Testcase"));
    testTabs->addWidget(new QLabel("Test Result"));
    testTabs->addStretch();
    auto btnExpand = new QPushButton("∧");
    btnExpand->setFlat(true);
    testTabs->addWidget(btnExpand);

    auto testOutput = new QTextEdit();
    testOutput->setReadOnly(true);
    testOutput->setObjectName("testOutput");

    testLayout->addLayout(testTabs);
    testLayout->addWidget(testOutput);

    auto rightSplitter = new QSplitter(Qt::Vertical);
    rightSplitter->addWidget(codeFrame);
    rightSplitter->addWidget(testFrame);
    rightLayout->addWidget(rightSplitter);

    mainSplitter->addWidget(leftContainer);
    mainSplitter->addWidget(rightContainer);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 1);

    rootLayout->addWidget(mainSplitter);

    // Логика переключения
    connect(btnPractice, &QPushButton::clicked, [=]() {
        contentStack->setCurrentIndex(0);
        btnPractice->setChecked(true);
        btnTheory->setChecked(false);
    });
    connect(btnTheory, &QPushButton::clicked, [=]() {
        contentStack->setCurrentIndex(1);
        btnTheory->setChecked(true);
        btnPractice->setChecked(false);
    });
    connect(btnBack, &QPushButton::clicked, this, &TaskWindow::fadeOut);

    // Стили
    setStyleSheet(R"(
        #TaskWindow { background-color: #f0f0f0; border: 1px solid #d0d0d0; }
        
        #tabHeader { background-color: #d6d6d6; border-bottom: 1px solid #bbb; }
        
        QPushButton#tabButton {
            border: none;
            padding: 10px 30px;
            font-weight: bold;
            background: transparent;
            border-right: 1px solid #ccc;
        }
        QPushButton#tabButton:checked { background-color: white; border-bottom: 2px solid #62639b; }
        
        QScrollArea { border: none; background-color: white; }
        
        #editorFrame, #testFrame {
            background-color: white;
            border: 1px solid #ccc;
            border-radius: 0px;
        }

        #codeEditor, #testOutput {
            border: 1px solid #eee;
            font-family: 'Consolas', monospace;
            font-size: 14px;
        }

        QPushButton#runButton {
            padding: 6px 20px;
            background: #eee;
            border: 1px solid #ccc;
            border-radius: 4px;
        }

        QPushButton#submitButton {
            padding: 6px 20px;
            background: #a8d5ba;
            border: 1px solid #8dbf9f;
            border-radius: 4px;
            font-weight: bold;
        }

        QPushButton#backButton {
            background-color: #ccc;
            border-radius: 20px;
            border: none;
        }
        QPushButton#backButton:hover { background-color: #bbb; }

        QSplitter::handle { background-color: #ccc; }
    )");
}