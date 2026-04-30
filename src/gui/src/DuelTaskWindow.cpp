#include "../include/DuelTaskWindow.hpp"

#include "../../core/include/services/DuelManager.hpp"
#include "../include/CppHighlighter.hpp"

#include <QDebug>
#include <QFont>
#include <QFrame>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QScrollBar>
#include <QSplitter>
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

void DuelTaskWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setFixedSize(1300, 900);
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
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0f172a, stop:1 #334155);
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }
    )");

    auto duelLayout = new QHBoxLayout(duelHeader);
    duelLayout->setContentsMargins(30, 0, 30, 0);

    labelTimer_ = new QLabel("TIME: 10:00");
    labelScore_ = new QLabel("WIN: +10 | LOSS: -5");

    labelTimer_->setFont(QFont("Outfit", 20, QFont::Black));
    labelScore_->setFont(QFont("Outfit", 12, QFont::Bold));

    labelTimer_->setStyleSheet("color: #ffffff; background: transparent;");
    labelScore_->setStyleSheet(
        "color: #94a3b8; background: transparent; text-transform: uppercase; letter-spacing: 1px;");

    duelLayout->addWidget(labelTimer_);
    duelLayout->addStretch();
    duelLayout->addWidget(labelScore_);
    rootLayout->addWidget(duelHeader);

    auto mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(4);

    practiceEdit_ = new QTextEdit();
    practiceEdit_->setReadOnly(true);
    practiceEdit_->setFrameStyle(QFrame::NoFrame);
    practiceEdit_->setFont(QFont("Inter", 12));
    practiceEdit_->setStyleSheet(R"(
        padding: 30px; 
        background-color: #ffffff; 
        border-right: 1px solid #e2e8f0;
        color: #334155;
        line-height: 1.6;
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
    new CppHighlighter(codeEditor_->document());
    codeEditor_->setObjectName("codeEditor");
    codeEditor_->setLineWrapMode(QTextEdit::WidgetWidth);
    QFont codeFont("Consolas", 13);
    codeEditor_->setFont(codeFont);

    QFontMetrics metrics(codeFont);
    codeEditor_->setTabStopDistance(4 * metrics.horizontalAdvance(' '));
    codeEditor_->installEventFilter(this);
    codeLayout->addWidget(codeEditor_);

    auto codeActions = new QHBoxLayout();
    btnRun_ = new QPushButton("Run");
    btnSubmit_ = new QPushButton("FINISH DUEL");
    btnSurrender_ = new QPushButton("SURRENDER");
    btnExit_ = new QPushButton("EXIT");

    btnRun_->setObjectName("runButton");
    btnSubmit_->setObjectName("submitButton");
    btnSurrender_->setObjectName("surrenderButton");
    btnExit_->setObjectName("exitButton");
    btnExit_->setVisible(false);

    btnRun_->setFixedSize(100, 45);
    btnSubmit_->setFixedSize(140, 45);
    btnSurrender_->setFixedSize(120, 45);
    btnExit_->setFixedSize(100, 45);

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

void DuelTaskWindow::setupStyles()
{
    setStyleSheet(R"(
        #DuelEditorWindow { background-color: #f8fafc; border: 1px solid #e2e8f0; border-radius: 12px; }
        #editorFrame, #testFrame { 
            background-color: white; 
            border: 1px solid #e2e8f0; 
            border-radius: 16px;
        }
        #codeEditor, #testOutput { background-color: transparent; border: none; padding: 10px; }
        
        QPushButton#runButton { 
            background-color: #ffffff; 
            border: 1px solid #e2e8f0;
            border-radius: 12px; 
            font-weight: 800; 
            color: #475569;
        }
        QPushButton#runButton:hover { background-color: #f1f5f9; border-color: #cbd5e1; }
        
        QPushButton#submitButton { 
            background-color: #3b82f6; 
            border-radius: 12px; 
            font-weight: 800; 
            border: none; 
            color: white; 
        }
        QPushButton#submitButton:hover { background-color: #2563eb; }
        
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
            border: none; 
        }
        QPushButton#exitButton:hover { background-color: #475569; }
        
        QLabel { font-weight: 800; color: #1e293b; font-size: 13px; }
    )");
}

void DuelTaskWindow::setTask(const cppforge::entities::CodingTask &task)
{
    currentTask_ = task;
    customTitleBar_->setTitle("DUEL: " + task.getTitle());
    practiceEdit_->setPlainText(task.getDescription());
    codeEditor_->setPlainText(task.getInitialCode());

    timeLeft_ = 600;
    currentScore_ = 1000;
    labelScore_->setText("WIN: +10 | LOSS: -5");

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

        if (timeLeft_ < 60)
        {
            labelTimer_->setStyleSheet("color: #ef4444; background: transparent;");
        }
        else
        {
            labelTimer_->setStyleSheet("color: #ffffff; background: transparent;");
        }

        if (timeLeft_ % 5 == 0 && currentScore_ > 50)
        {
            currentScore_ -= 1;
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
                        testOutput_->append("<pre style='background:#f4f4f4; padding:10px;'>" + result.getOutput() +
                                            "</pre>");
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