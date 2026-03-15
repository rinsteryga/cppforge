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

void TaskWindow::centerWindow()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect adj = screen->availableGeometry();
        move(adj.x() + (adj.width() - width()) / 2, adj.y() + (adj.height() - height()) / 2);
    }
}

void TaskWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setFixedSize(1250, 850);
    setObjectName("TaskWindow");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));

    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setTitle("Название");
    customTitleBar_->setIcon(windowIcon());
    rootLayout->addWidget(customTitleBar_.get());

    QFrame* line = new QFrame();
    line->setFixedHeight(1);
    line->setStyleSheet("background-color: #999;");
    rootLayout->addWidget(line);

    auto mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(2);

    auto leftContainer = new QWidget();
    auto leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    auto tabHeader = new QFrame();
    tabHeader->setObjectName("tabHeader");
    auto tabLayout = new QHBoxLayout(tabHeader);
    tabLayout->setContentsMargins(15, 0, 0, 0);
    tabLayout->setSpacing(20);

    auto btnPractice = new QPushButton("✧ Практика");
    auto btnTheory = new QPushButton("✧ Теория");
    btnPractice->setObjectName("tabButton");
    btnTheory->setObjectName("tabButton");
    btnPractice->setCheckable(true);
    btnTheory->setCheckable(true);
    btnPractice->setChecked(true);

    tabLayout->addWidget(btnPractice);
    tabLayout->addWidget(btnTheory);
    tabLayout->addStretch();
    
    auto btnCollapse = new QPushButton("<");
    btnCollapse->setFixedSize(30, 30);
    btnCollapse->setFlat(true);
    tabLayout->addWidget(btnCollapse);
    leftLayout->addWidget(tabHeader);

    auto contentStack = new QStackedWidget();
    
    auto practiceScroll = new QScrollArea();
    auto practiceContent = new QLabel("ТЕКСТ ПРАКТИКИ\n\nТЕКСТ ЗАДАНИЯ\nТЕКСТ ТЕКСТ");
    practiceContent->setAlignment(Qt::AlignTop);
    practiceContent->setContentsMargins(30, 30, 30, 30);
    practiceScroll->setWidget(practiceContent);
    practiceScroll->setWidgetResizable(true);

    auto theoryScroll = new QScrollArea();
    auto theoryContent = new QLabel("ТЕКСТ ТЕОРИИ");
    theoryContent->setAlignment(Qt::AlignTop);
    theoryContent->setContentsMargins(30, 30, 30, 30);
    theoryScroll->setWidget(theoryContent);
    theoryScroll->setWidgetResizable(true);

    contentStack->addWidget(practiceScroll);
    contentStack->addWidget(theoryScroll);
    leftLayout->addWidget(contentStack);

    auto footerLeft = new QHBoxLayout();
    footerLeft->setContentsMargins(15, 15, 15, 15);
    auto btnBack = new QPushButton();
    btnBack->setFixedSize(45, 45);
    btnBack->setObjectName("backButton");
    footerLeft->addWidget(btnBack);
    footerLeft->addStretch();
    leftLayout->addLayout(footerLeft);

    auto rightContainer = new QWidget();
    auto rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(15, 15, 15, 15);
    rightLayout->setSpacing(15);

    auto rightSplitter = new QSplitter(Qt::Vertical);

    auto codeFrame = new QFrame();
    codeFrame->setObjectName("editorFrame");
    auto codeLayout = new QVBoxLayout(codeFrame);
    codeLayout->addWidget(new QLabel("<\\> Code"));
    auto codeEditor = new QTextEdit();
    codeEditor->setObjectName("codeEditor");
    codeLayout->addWidget(codeEditor);

    auto codeActions = new QHBoxLayout();
    auto btnRun = new QPushButton("Run");
    auto btnSubmit = new QPushButton("Submit");
    btnRun->setObjectName("runButton");
    btnSubmit->setObjectName("submitButton");
    codeActions->addStretch();
    codeActions->addWidget(btnRun);
    codeActions->addWidget(btnSubmit);
    codeLayout->addLayout(codeActions);

    auto testFrame = new QFrame();
    testFrame->setObjectName("testFrame");
    auto testLayout = new QVBoxLayout(testFrame);
    auto testTabs = new QHBoxLayout();
    testTabs->addWidget(new QLabel("✧ Testcase"));
    testTabs->addWidget(new QLabel("✧ Test Result"));
    testTabs->addStretch();
    testTabs->addWidget(new QLabel("∧"));
    testLayout->addLayout(testTabs);
    auto testOutput = new QTextEdit();
    testOutput->setReadOnly(true);
    testOutput->setObjectName("testOutput");
    testLayout->addWidget(testOutput);

    rightSplitter->addWidget(codeFrame);
    rightSplitter->addWidget(testFrame);
    rightSplitter->setStretchFactor(0, 3);
    rightSplitter->setStretchFactor(1, 1);
    rightLayout->addWidget(rightSplitter);

    mainSplitter->addWidget(leftContainer);
    mainSplitter->addWidget(rightContainer);
    rootLayout->addWidget(mainSplitter);

    connect(btnPractice, &QPushButton::clicked, [=](){ contentStack->setCurrentIndex(0); btnPractice->setChecked(true); btnTheory->setChecked(false); });
    connect(btnTheory, &QPushButton::clicked, [=](){ contentStack->setCurrentIndex(1); btnTheory->setChecked(true); btnPractice->setChecked(false); });
    connect(btnBack, &QPushButton::clicked, this, &TaskWindow::fadeOut);

    setupStyles();
}

void TaskWindow::setupStyles()
{
    setStyleSheet(R"(
        #TaskWindow { background-color: white; border: 1px solid #777; }
        #tabHeader { background-color: #eeeeee; border-bottom: 1px solid #bbbbbb; min-height: 40px; }
        QPushButton#tabButton { border: none; background: transparent; font-weight: bold; font-size: 13px; padding: 5px 10px; }
        QPushButton#tabButton:checked { border-bottom: 2px solid black; }
        #editorFrame, #testFrame { background-color: white; border: 2px solid #dddddd; border-radius: 20px; }
        #codeEditor, #testOutput { border: none; font-family: 'Consolas'; }
        QPushButton#runButton, QPushButton#submitButton { border-radius: 12px; padding: 5px 20px; font-weight: bold; }
        QPushButton#runButton { background-color: #f0f0f0; border: 1px solid #ccc; }
        QPushButton#submitButton { background-color: #b8e2c8; border: none; }
        QPushButton#backButton { background-color: #d9d9d9; border-radius: 22px; border: none; }
        QSplitter::handle { background-color: #bbbbbb; }
        QScrollArea { border: none; background: white; }
    )");
}

void TaskWindow::fadeIn() {
    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(300);
    transitionAnimation_->setStartValue(0.0);
    transitionAnimation_->setEndValue(1.0);
    transitionAnimation_->start();
}

void TaskWindow::fadeOut() {
    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(200);
    transitionAnimation_->setStartValue(1.0);
    transitionAnimation_->setEndValue(0.0);
    connect(transitionAnimation_.get(), &QPropertyAnimation::finished, this, [this]() { hide(); emit windowClosed(); });
    transitionAnimation_->start();
}

void TaskWindow::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    fadeIn();
}