#include "../include/DuelTaskWindow.hpp"

#include "../../core/include/services/DuelManager.hpp"
#include "../include/CppHighlighter.hpp"
#include "../include/WindowStateManager.hpp"

#include <QDebug>
#include <QFont>
#include <QFrame>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QStyle>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>

DuelTaskWindow::DuelTaskWindow(cppforge::services::DuelManager *manager, QWidget *parent)
    : QWidget(parent), duelManager_(manager), runner_(std::make_unique<cppforge::services::CodeRunner>(this)),
      analyzer_(std::make_unique<cppforge::services::StaticAnalyzer>())
{
    setupUI();
    setupStyles();
    duelTimer_ = new QTimer(this);
    connect(duelTimer_, &QTimer::timeout, this, &DuelTaskWindow::onTick);
}

DuelTaskWindow::~DuelTaskWindow() = default;

void DuelTaskWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setMinimumSize(800, 580);
    resize(1200, 800);
    setObjectName("DuelEditorWindow");

    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    rootLayout->addWidget(customTitleBar_.get());

    auto duelHeader = new QFrame();
    duelHeader->setFixedHeight(70);
    duelHeader->setObjectName("duelHeader");
    duelHeader->setStyleSheet(R"(
        #duelHeader {
            background-color: palette(alternate-base);
            border-bottom: 1px solid palette(mid);
            border-top-left-radius: 19px;
            border-top-right-radius: 19px;
        }
    )");

    auto duelLayout = new QHBoxLayout(duelHeader);
    duelLayout->setContentsMargins(30, 0, 30, 0);

    labelTimer_ = new QLabel("TIME: 10:00");
    labelScore_ = new QLabel("WIN: +10 | LOSS: -5");
    labelTimer_->setObjectName("labelTimer");
    labelScore_->setObjectName("labelScore");

    labelTimer_->setFont(QFont("Outfit", 20, QFont::Black));
    labelScore_->setFont(QFont("Outfit", 12, QFont::Bold));

    duelLayout->addWidget(labelTimer_);
    duelLayout->addStretch();
    duelLayout->addWidget(labelScore_);
    rootLayout->addWidget(duelHeader);

    auto mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(4);

    practiceEdit_ = new QTextEdit();
    practiceEdit_->setObjectName("practiceEdit");
    practiceEdit_->setReadOnly(true);
    practiceEdit_->setFrameStyle(QFrame::NoFrame);
    practiceEdit_->setFont(QFont("Inter", 12));
    practiceEdit_->setStyleSheet(R"(
        padding: 30px; 
        background-color: palette(base); 
        border-right: 1px solid palette(mid);
        color: palette(text);
    )");
    practiceEdit_->installEventFilter(this);

    auto rightContainer = new QWidget();
    auto rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(20, 20, 20, 20);

    auto rightSplitter = new QSplitter(Qt::Vertical);

    auto codeFrame = new QFrame();
    codeFrame->setObjectName("editorFrame");
    auto codeLayout = new QVBoxLayout(codeFrame);
    codeLayout->addWidget(new QLabel("<\\> DUEL EDITOR"));

    codeEditor_ = new QTextEdit();
    codeEditor_->setObjectName("codeEditor");
    codeEditor_->setFont(QFont("Consolas", 12));

    highlighter_ = std::make_unique<CppHighlighter>(codeEditor_->document());
    if (themeService_)
    {
        highlighter_->setTheme(themeService_->getCurrentTheme() == cppforge::services::Theme::Dark);
    }
    codeEditor_->setLineWrapMode(QTextEdit::WidgetWidth);
    QFont codeFont("Consolas", 13);
    codeEditor_->setFont(codeFont);

    QFontMetrics metrics(codeFont);
    codeEditor_->setTabStopDistance(4 * metrics.horizontalAdvance(' '));
    codeEditor_->installEventFilter(this);
    codeLayout->addWidget(codeEditor_);

    auto codeActions = new QHBoxLayout();
    btnRun_ = new QPushButton("Run");
    btnSubmit_ = new QPushButton("SUBMIT");
    btnSurrender_ = new QPushButton("SURRENDER");
    btnExit_ = new QPushButton("EXIT");

    btnRun_->setObjectName("runButton");
    btnSubmit_->setObjectName("submitButton");
    btnSurrender_->setObjectName("surrenderButton");
    btnExit_->setObjectName("exitButton");
    btnExit_->setVisible(false);

    btnRun_->setMinimumSize(80, 40);
    btnRun_->setMaximumSize(120, 50);
    btnSubmit_->setMinimumSize(120, 40);
    btnSubmit_->setMaximumSize(160, 50);
    btnSurrender_->setMinimumSize(100, 40);
    btnSurrender_->setMaximumSize(140, 50);
    btnExit_->setMinimumSize(80, 40);
    btnExit_->setMaximumSize(120, 50);

    codeActions->addStretch();
    codeActions->addWidget(btnRun_);
    codeActions->addWidget(btnSubmit_);
    codeActions->addWidget(btnSurrender_);
    codeActions->addWidget(btnExit_);
    codeLayout->addLayout(codeActions);

    auto testFrame = new QFrame();
    testFrame->setObjectName("testFrame");
    auto testLayout = new QVBoxLayout(testFrame);
    testLayout->addWidget(new QLabel("✧ TEST RESULTS"));

    testOutput_ = new QTextEdit();
    testOutput_->setReadOnly(true);
    testOutput_->setObjectName("testOutput");
    testOutput_->setFont(QFont("Consolas", 12));
    testOutput_->installEventFilter(this);
    testLayout->addWidget(testOutput_);

    rightSplitter->addWidget(codeFrame);
    rightSplitter->addWidget(testFrame);
    rightSplitter->setStretchFactor(0, 3);
    rightSplitter->setStretchFactor(1, 1);
    rightLayout->addWidget(rightSplitter);

    mainSplitter->addWidget(practiceEdit_);
    mainSplitter->addWidget(rightContainer);
    rootLayout->addWidget(mainSplitter);

    connect(btnRun_, &QPushButton::clicked, this, &DuelTaskWindow::onRunClicked);
    connect(btnSubmit_, &QPushButton::clicked, this, &DuelTaskWindow::onSubmitClicked);
    connect(btnSurrender_, &QPushButton::clicked, this, &DuelTaskWindow::onSurrenderClicked);
    connect(btnExit_, &QPushButton::clicked, this, &DuelTaskWindow::onExitClicked);
}

void DuelTaskWindow::setThemeService(cppforge::services::ThemeService *service)
{
    themeService_ = service;
    if (themeService_)
    {
        connect(themeService_, &cppforge::services::ThemeService::themeChanged, this,
                [this](cppforge::services::Theme theme)
                {
                    if (highlighter_)
                    {
                        highlighter_->setTheme(theme == cppforge::services::Theme::Dark);
                    }
                    if (customTitleBar_)
                    {
                        QString iconPath = (theme == cppforge::services::Theme::Dark) ? ":/icons/main_logo_dark.ico"
                                                                                      : ":/icons/main_logo.ico";
                        customTitleBar_->setIcon(QIcon(iconPath));
                        customTitleBar_->setThemeService(themeService_);
                    }
                    setupStyles(theme == cppforge::services::Theme::Dark);
                });

        if (highlighter_)
        {
            highlighter_->setTheme(themeService_->getCurrentTheme() == cppforge::services::Theme::Dark);
        }

        if (customTitleBar_)
        {
            QString iconPath = (themeService_->getCurrentTheme() == cppforge::services::Theme::Dark)
                                   ? ":/icons/main_logo_dark.ico"
                                   : ":/icons/main_logo.ico";
            customTitleBar_->setIcon(QIcon(iconPath));
            customTitleBar_->setThemeService(themeService_);
        }
        setupStyles(themeService_->getCurrentTheme() == cppforge::services::Theme::Dark);
    }
}

void DuelTaskWindow::setupStyles(std::optional<bool> isDarkOverride)
{
    bool isDark = (palette().color(QPalette::Window).lightness() < 128);

    QString accentColor = isDark ? "#0e639c" : "#62639b";
    QString hoverColor = isDark ? "#1177bb" : "#f3e8ff";
    QString hoverText = isDark ? "white" : "black";

    QString cssStyle =
        R"(
        #DuelEditorWindow { 
            background-color: palette(window); 
            border: 1px solid palette(mid); 
            border-radius: 20px; 
        }
        #editorFrame, #testFrame { 
            background-color: palette(base); 
            border: 1px solid palette(mid); 
            border-radius: 16px;
        }
        #codeEditor, #testOutput { background-color: transparent; border: none; padding: 10px; color: palette(text); }
        
        QTextEdit#practiceEdit {
            padding: 30px;
        }
        QTextEdit#practiceEdit h1 { color: )" +
        accentColor +
        R"(; font-family: 'Outfit', 'Inter', sans-serif; font-size: 26px; font-weight: 800; margin-bottom: 15px; }
        QTextEdit#practiceEdit h2 { color: )" +
        accentColor +
        R"(; font-family: 'Outfit', 'Inter', sans-serif; font-size: 20px; font-weight: 700; border-left: 5px solid )" +
        accentColor + R"(; padding-left: 12px; margin-top: 25px; margin-bottom: 10px; }
        QTextEdit#practiceEdit h3 { color: )" +
        accentColor +
        R"(; font-family: 'Outfit', 'Inter', sans-serif; font-size: 18px; font-weight: 600; margin-top: 20px; }
        QTextEdit#practiceEdit p { line-height: 1.6; margin-bottom: 12px; }
        QTextEdit#practiceEdit li { margin-bottom: 8px; }
        QTextEdit#practiceEdit pre { background-color: palette(alternate-base); padding: 15px; border-radius: 10px; border: 1px solid palette(mid); font-family: 'Consolas', monospace; font-size: 13px; margin: 10px 0; }
        QTextEdit#practiceEdit code { background-color: palette(alternate-base); padding: 2px 6px; border-radius: 4px; font-family: 'Consolas', monospace; font-weight: bold; }

        QPushButton#runButton { 
            background-color: palette(base); 
            border: 1px solid palette(mid);
            border-radius: 12px; 
            font-weight: 800; 
            color: palette(text);
        }
        QPushButton#runButton:hover { background-color: )" +
        hoverColor + R"( !important; color: )" + hoverText + R"( !important; border-color: )" + hoverColor +
        R"( !important; }
        
        QPushButton#submitButton { 
            background-color: )" +
        accentColor + R"(; 
            border-radius: 12px; 
            font-weight: 800; 
            border: none; 
            color: white; 
        }
        QPushButton#submitButton:hover { background-color: )" +
        hoverColor + R"( !important; color: )" + hoverText + R"( !important; border-color: )" + hoverColor +
        R"( !important; }
        
        #labelTimer { color: #ef4444; font-weight: 800; }
        #labelScore { color: palette(window-text); font-weight: 800; }

        QPushButton#surrenderButton { 
            background-color: #ef4444; 
            color: white; 
            border-radius: 12px; 
            font-weight: 800; 
            border: none; 
        }
        QPushButton#surrenderButton:hover { background-color: #dc2626; }
        
        QPushButton#exitButton { 
            background-color: #64748b; 
            color: white; 
            border-radius: 12px; 
            font-weight: 800; 
            border: 1px solid palette(mid); 
        }
        QPushButton#exitButton:hover { background-color: #475569; }
        
        QLabel { font-weight: 800; color: palette(text); font-size: 13px; }
        #labelTimer { color: #ef4444; font-size: 24px; font-weight: bold; }
        #labelScore { font-size: 14px; font-weight: bold; color: palette(text); }
    )";

    setStyleSheet(cssStyle);

    style()->unpolish(this);
    style()->polish(this);
    update();
}

void DuelTaskWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void DuelTaskWindow::setTask(const cppforge::entities::CodingTask &task)
{
    currentTask_ = task;
    customTitleBar_->setTitle("DUEL: " + task.getTitle());
    practiceEdit_->setMarkdown(task.getDescription());
    codeEditor_->setPlainText(task.getInitialCode());

    timeLeft_ = 600;
    currentScore_ = 10;
    labelScore_->setText("WIN: +10 | BONUS: +10");

    testOutput_->clear();
    testOutput_->append("<b style='color:#3498db;'>[DUEL]</b> Task received. Start solving!");

    duelTimer_->start(1000);
}

void DuelTaskWindow::onTick()
{
    if (timeLeft_ > 0)
    {
        timeLeft_--;
        int min = timeLeft_ / 60;
        int sec = timeLeft_ % 60;
        labelTimer_->setText(QString("%1:%2").arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0')));

        labelTimer_->setStyleSheet("color: #ef4444; background: transparent; font-weight: bold;");

        if (timeLeft_ % 6 == 0 && currentScore_ < 20)
        {
            currentScore_ = 10 + (timeLeft_ / 60);
            labelScore_->setText(QString("WIN: +10 | BONUS: +%1").arg(timeLeft_ / 60));
        }
    }
    else
    {
        duelTimer_->stop();
        labelTimer_->setText("TIME'S UP!");
        if (duelManager_)
            duelManager_->finishDuel(0);
    }
}

void DuelTaskWindow::onRunClicked()
{
    if (currentTask_.getId() == 0)
        return;

    QString code = codeEditor_->toPlainText();
    testOutput_->clear();
    testOutput_->append("<b style='color:#3498db;'>[RUN]</b> Compiling...");

    std::vector<cppforge::entities::TestCase> runTests;
    if (!currentTask_.getTestCases().empty())
    {
        runTests.push_back(*currentTask_.getTestCases().begin());
    }

    auto watcher = new QFutureWatcher<cppforge::entities::ExecutionResult>(this);
    connect(watcher, &QFutureWatcher<cppforge::entities::ExecutionResult>::finished,
            [this, watcher]()
            {
                auto result = watcher->result();
                if (testOutput_)
                {
                    if (result.getErrors().isEmpty())
                    {
                        testOutput_->append("<span style='color:#27ae60; font-weight:bold;'>Program Output:</span>");
                        testOutput_->append("<pre style='background:transparent; padding:10px; color:palette(text);'>" +
                                            result.getOutput() + "</pre>");
                    }
                    else
                    {
                        testOutput_->append(
                            "<span style='color:#e74c3c; font-weight:bold;'>Compilation/Execution Error:</span>");
                        testOutput_->append("<pre style='color:#c0392b;'>" + result.getErrors() + "</pre>");
                    }
                }
                watcher->deleteLater();
            });

    watcher->setFuture(runner_->runAsync(code, runTests));
}

void DuelTaskWindow::onSubmitClicked()
{
    QString code = codeEditor_->toPlainText();
    testOutput_->clear();
    testOutput_->append("<b style='color:#f39c12;'>[SUBMIT]</b> Final verification of all tests...");

    auto violation = analyzer_->analyze(currentTask_, code);
    if (violation.has_value())
    {
        testOutput_->append("<span style='color:#e74c3c;'>[Analysis] " + violation.value() + "</span>");
        return;
    }

    std::vector<cppforge::entities::TestCase> allTests(currentTask_.getTestCases().begin(),
                                                       currentTask_.getTestCases().end());

    auto watcher = new QFutureWatcher<cppforge::entities::ExecutionResult>(this);
    connect(watcher, &QFutureWatcher<cppforge::entities::ExecutionResult>::finished,
            [this, watcher]()
            {
                auto result = watcher->result();
                if (result.isSuccess())
                {
                    duelTimer_->stop();
                    codeEditor_->setReadOnly(true);

                    testOutput_->append("<h2 style='color:#27ae60;'>✔ TESTS PASSED!</h2>");
                    testOutput_->append("<p>Sending results to server...</p>");

                    if (duelManager_)
                    {
                        duelManager_->finishDuel(currentScore_);
                    }
                }
                else
                {
                    testOutput_->append("<h3 style='color:#e74c3c;'>✘ Solution rejected</h3>");
                    if (!result.getErrors().isEmpty())
                    {
                        testOutput_->append("<pre style='color:red;'>" + result.getErrors() + "</pre>");
                    }
                }
                watcher->deleteLater();
            });

    watcher->setFuture(runner_->runAsync(code, allTests));
}

void DuelTaskWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    auto anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setDuration(300);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void DuelTaskWindow::showFinalResult(const QString &winnerName, int score)
{
    QString cleanWinner = winnerName.trimmed();
    QString cleanLocal = m_localNickname.trimmed();

    qDebug() << "[DUEL] Result comparison -> Local:" << cleanLocal << "Winner:" << cleanWinner;

    bool isMe = (cleanWinner == cleanLocal);

    if (isMe)
    {
        labelScore_->setText(QString("VICTORY! Your score: %1").arg(score));
        labelScore_->setStyleSheet("color: #2ecc71; font-weight: bold; font-size: 20px;");
    }
    else
    {
        labelScore_->setText(QString("DEFEAT. Winner: %1").arg(cleanWinner));
        labelScore_->setStyleSheet("color: #e74c3c; font-weight: bold; font-size: 20px;");
    }

    if (duelTimer_)
        duelTimer_->stop();
    codeEditor_->setReadOnly(true);

    btnRun_->setEnabled(false);
    btnSubmit_->setEnabled(false);
    btnSurrender_->setVisible(false);
    btnExit_->setVisible(true);
}

void DuelTaskWindow::onSurrenderClicked()
{
    if (duelManager_)
        duelManager_->surrender();
}

void DuelTaskWindow::onExitClicked()
{
    WindowStateManager::instance().captureState(this);
    emit sessionClosed();
    this->close();
}

bool DuelTaskWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == codeEditor_ && event->type() == QEvent::KeyPress)
    {
        auto *keyEvent = dynamic_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab)
        {
            codeEditor_->insertPlainText("    ");
            return true;
        }

        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            QString currentLine = codeEditor_->textCursor().block().text();
            QString indent;
            for (QChar c : currentLine)
            {
                if (c.isSpace())
                    indent += c;
                else
                    break;
            }

            if (currentLine.trimmed().endsWith('{'))
            {
                codeEditor_->insertPlainText("\n" + indent + "    ");
            }
            else
            {
                codeEditor_->insertPlainText("\n" + indent);
            }
            return true;
        }

        struct BracketPair
        {
            QChar open;
            QChar close;
        };
        static const QMap<int, BracketPair> pairs = {{Qt::Key_ParenLeft, {'(', ')'}},
                                                     {Qt::Key_BracketLeft, {'[', ']'}},
                                                     {Qt::Key_BraceLeft, {'{', '}'}},
                                                     {Qt::Key_QuoteDbl, {'"', '"'}},
                                                     {Qt::Key_QuoteLeft, {'\'', '\''}}};

        if (pairs.contains(keyEvent->key()))
        {
            const auto &p = pairs[keyEvent->key()];
            codeEditor_->insertPlainText(QString(p.open) + p.close);
            codeEditor_->moveCursor(QTextCursor::Left);
            return true;
        }
    }

    if (event->type() == QEvent::Wheel)
    {
        auto *wheelEvent = static_cast<QWheelEvent *>(event);
        if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier)
        {
            if (wheelEvent->angleDelta().y() > 0)
            {
                codeEditor_->zoomIn(1);
            }
            else
            {
                codeEditor_->zoomOut(1);
            }
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void DuelTaskWindow::setLocalNickname(const QString &name)
{
    m_localNickname = name;
}
