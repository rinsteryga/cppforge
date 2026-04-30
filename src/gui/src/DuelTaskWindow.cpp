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

    QFrame *line = new QFrame();
    line->setFixedHeight(1);
    line->setStyleSheet("background-color: #999;");
    rootLayout->addWidget(line);

    auto duelHeader = new QFrame();
    duelHeader->setFixedHeight(65);
    duelHeader->setObjectName("duelHeader");
    duelHeader->setStyleSheet("background-color: #2c3e50; border-bottom: 2px solid #1a252f;");

    auto duelLayout = new QHBoxLayout(duelHeader);
    duelLayout->setContentsMargins(30, 0, 30, 0);

    labelTimer_ = new QLabel("TIME: 10:00");
    labelScore_ = new QLabel("SCORE: 1000");

    QFont duelFont("Consolas", 18, QFont::Bold);
    labelTimer_->setFont(duelFont);
    labelScore_->setFont(duelFont);
    labelTimer_->setStyleSheet("color: #e74c3c;");
    labelScore_->setStyleSheet("color: #f1c40f;");

    duelLayout->addWidget(labelTimer_);
    duelLayout->addStretch();
    duelLayout->addWidget(labelScore_);
    rootLayout->addWidget(duelHeader);

    auto mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(4);

    practiceEdit_ = new QTextEdit();
    practiceEdit_->setReadOnly(true);
    practiceEdit_->setFrameStyle(QFrame::NoFrame);
    practiceEdit_->setFont(QFont("Roboto", 13));
    practiceEdit_->setStyleSheet("padding: 25px; background-color: #fdfdfd;");
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
    QFont codeFont("Consolas", 13);
    codeEditor_->setFont(codeFont);

    QFontMetrics metrics(codeFont);
    codeEditor_->setTabStopDistance(4 * metrics.horizontalAdvance(' '));
    codeEditor_->installEventFilter(this);
    codeLayout->addWidget(codeEditor_);

    auto codeActions = new QHBoxLayout();
    auto btnRun = new QPushButton("Run");
    auto btnSubmit = new QPushButton("FINISH DUEL");

    btnRun->setObjectName("runButton");
    btnSubmit->setObjectName("submitButton");
    btnRun->setFixedSize(120, 50);
    btnSubmit->setFixedSize(160, 50);

    codeActions->addStretch();
    codeActions->addWidget(btnRun);
    codeActions->addWidget(btnSubmit);
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

    connect(btnRun, &QPushButton::clicked, this, &DuelTaskWindow::onRunClicked);
    connect(btnSubmit, &QPushButton::clicked, this, &DuelTaskWindow::onSubmitClicked);
}

void DuelTaskWindow::setupStyles()
{
    setStyleSheet(R"(
        #DuelEditorWindow { background-color: white; border: 1px solid #777; }
        #editorFrame, #testFrame { background-color: white; border: 1px solid #ccc; border-radius: 8px; }
        #codeEditor, #testOutput { border: none; }
        QPushButton#runButton { background-color: #f0f0f0; border-radius: 8px; font-weight: bold; border: 1px solid #ccc; }
        QPushButton#submitButton { 
            background-color: #e67e22; 
            border-radius: 8px; 
            font-weight: bold; 
            border: none; 
            color: white; 
        }
        QPushButton#submitButton:hover { background-color: #d35400; }
        QLabel { font-weight: bold; color: #555; }
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
    labelScore_->setText("SCORE: 1000");

    testOutput_->clear();
    testOutput_->append("<b style='color:#3498db;'>[DUEL]</b> Задача получена. Приступайте к решению!");

    duelTimer_->start(1000);
}

void DuelTaskWindow::onTick()
{
    if (timeLeft_ > 0)
    {
        timeLeft_--;
        int m = timeLeft_ / 60;
        int s = timeLeft_ % 60;
        labelTimer_->setText(QString("TIME: %1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0')));

        if (timeLeft_ % 5 == 0 && currentScore_ > 50)
        {
            currentScore_ -= 1;
            labelScore_->setText(QString("SCORE: %1").arg(currentScore_));
        }
    }
    else
    {
        duelTimer_->stop();
        labelTimer_->setText("TIME'S UP!");
        onSubmitClicked();
    }
}

void DuelTaskWindow::onRunClicked()
{
    if (currentTask_.getId() == 0)
        return;

    QString code = codeEditor_->toPlainText();
    testOutput_->clear();
    testOutput_->append("<b style='color:#3498db;'>[RUN]</b> Компиляция...");

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
                        testOutput_->append("<span style='color:#27ae60; font-weight:bold;'>Вывод программы:</span>");
                        testOutput_->append("<pre style='background:#f4f4f4; padding:10px;'>" + result.getOutput() +
                                            "</pre>");
                    }
                    else
                    {
                        testOutput_->append(
                            "<span style='color:#e74c3c; font-weight:bold;'>Ошибка компиляции/выполнения:</span>");
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
    testOutput_->append("<b style='color:#f39c12;'>[SUBMIT]</b> Финальная проверка всех тестов...");

    auto violation = analyzer_->analyze(currentTask_, code);
    if (violation.has_value())
    {
        testOutput_->append("<span style='color:#e74c3c;'>[Анализ] " + violation.value() + "</span>");
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

                    testOutput_->append("<h2 style='color:#27ae60;'>✔ ТЕСТЫ ПРОЙДЕНЫ!</h2>");
                    testOutput_->append("<p>Результаты отправляются на сервер...</p>");

                    if (duelManager_)
                    {
                        duelManager_->finishDuel(currentScore_);
                    }
                }
                else
                {
                    testOutput_->append("<h3 style='color:#e74c3c;'>✘ Решение не принято</h3>");
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
        labelScore_->setText(QString("ПОБЕДА! Ваш счет: %1").arg(score));
        labelScore_->setStyleSheet("color: #2ecc71; font-weight: bold; font-size: 20px;");
    }
    else
    {
        labelScore_->setText(QString("ПОРАЖЕНИЕ. Победил %1").arg(cleanWinner));
        labelScore_->setStyleSheet("color: #e74c3c; font-weight: bold; font-size: 20px;");
    }

    if (duelTimer_)
        duelTimer_->stop();
    codeEditor_->setReadOnly(true);
}

bool DuelTaskWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == codeEditor_ && event->type() == QEvent::KeyPress)
    {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab)
        {
            codeEditor_->insertPlainText("    ");
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