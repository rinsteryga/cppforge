#include "../include/TaskWindow.hpp"

#include "../include/CppHighlighter.hpp"
#include "../include/CustomTitleBar.hpp"
#include "../include/WindowStateManager.hpp"

#include <QDebug>
#include <QFrame>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QSqlError>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QStyleOption>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>

#include <optional>
#include <set>

void applyTextFormatting(QTextEdit *editor)
{
    if (!editor)
        return;
    QTextCursor cursor = editor->textCursor();
    QTextBlockFormat blockFormat;
    blockFormat.setBottomMargin(15);
    blockFormat.setLineHeight(145, QTextBlockFormat::ProportionalHeight);
    cursor.select(QTextCursor::Document);
    cursor.setBlockFormat(blockFormat);
}

TaskWindow::TaskWindow(QWidget *parent)
    : QWidget(parent), runner_(std::make_unique<cppforge::services::CodeRunner>(this)),
      analyzer_(std::make_unique<cppforge::services::StaticAnalyzer>())
{
    setupUI();
    setWindowOpacity(0.0);
    QTimer::singleShot(50, this, &TaskWindow::centerWindow);
}

TaskWindow::~TaskWindow() = default;

void TaskWindow::setTask(const cppforge::entities::CodingTask &task)
{
    currentTask_ = task;
    hasCodingTask_ = true;
    currentModuleParentId_ = -1;

    customTitleBar_->setTitle("Task: " + task.getTitle());

    if (practiceEdit_)
    {
        practiceEdit_->setMarkdown(task.getDescription());
        applyTextFormatting(practiceEdit_);
    }

    if (btnNext_)
        btnNext_->hide();
    if (btnPrev_)
        btnPrev_->hide();

    currentModuleId_ = -1;

    if (theoryEdit_)
    {
        theoryEdit_->setMarkdown("Theoretical material is not provided for this task.");
        applyTextFormatting(theoryEdit_);
    }

    if (codeEditor_)
    {
        codeEditor_->setPlainText(task.getInitialCode());
        codeEditor_->setReadOnly(false);
    }

    if (btnRun_)
    {
        btnRun_->setVisible(true);
    }

    if (btnSubmit_)
    {
        btnSubmit_->setEnabled(true);
        btnSubmit_->setText("Submit");
        btnSubmit_->setStyleSheet("");
    }

    if (testOutput_)
    {
        testOutput_->clear();
    }
}

void TaskWindow::loadModule(int lessonId)
{
    if (btnNext_)
        btnNext_->show();
    if (btnPrev_)
        btnPrev_->show();

    if (!courseService_)
    {
        return;
    }

    currentModuleId_ = lessonId;

    if (testOutput_)
    {
        testOutput_->clear();
    }

    if (btnSubmit_)
    {
        btnSubmit_->setEnabled(true);
        btnSubmit_->setText("Submit");
        btnSubmit_->setStyleSheet("");
    }

    auto dataOpt = courseService_->getTaskWindowData(currentUserId_, lessonId);
    if (!dataOpt.has_value())
        return;

    auto data = dataOpt.value();
    QString title = data.title;
    QString theory = data.theoryContent;
    bool isCompleted = data.isCompleted;
    currentModuleParentId_ = data.moduleId;

    customTitleBar_->setTitle(title);
    if (theoryEdit_)
    {
        theoryEdit_->setMarkdown(theory);
        applyTextFormatting(theoryEdit_);
    }

    if (data.taskId.has_value())
    {
        hasCodingTask_ = true;
        uint64_t taskId = data.taskId.value();
        QString practiceDesc = data.practiceDescription.value_or("");
        QString initCode = data.initialCode.value_or("");

        if (btnRun_)
            btnRun_->setVisible(true);
        if (practiceEdit_)
        {
            practiceEdit_->setMarkdown(practiceDesc);
            applyTextFormatting(practiceEdit_);
        }

        if (data.previousCode.has_value())
        {
            codeEditor_->setPlainText(data.previousCode.value());
        }
        else
        {
            codeEditor_->setPlainText(initCode);
        }

        if (isCompleted && testOutput_)
        {
            testOutput_->append("<span style='color:#27ae60; font-weight:bold;'>[Status] This task has already been "
                                "completed correctly.</span>");
            if (btnSubmit_)
                btnSubmit_->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
        }

        codeEditor_->setReadOnly(false);

        currentTask_ =
            cppforge::entities::CodingTask(taskId, static_cast<uint64_t>(lessonId), title, practiceDesc, initCode,
                                           data.testCases, 1000, 256, data.whitelist, data.blacklist);
    }
    else
    {
        hasCodingTask_ = false;
        if (practiceEdit_)
        {
            practiceEdit_->setMarkdown("Practical tasks are not provided for this module.");
            applyTextFormatting(practiceEdit_);
        }
        codeEditor_->setPlainText("// Theory only material.");
        codeEditor_->setReadOnly(true);
        if (btnRun_)
            btnRun_->setVisible(false);

        if (btnSubmit_)
        {
            btnSubmit_->setText("Studied");
            if (isCompleted)
            {
                if (testOutput_)
                    testOutput_->append(
                        "<span style='color:#27ae60; font-weight:bold;'>[Status] Theory studied.</span>");
                btnSubmit_->setEnabled(true);
                btnSubmit_->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
            }
            else
            {
                btnSubmit_->setEnabled(false);
                btnSubmit_->setStyleSheet("background-color: #95a5a6; color: white; font-weight: bold; border: none;");

                QTimer::singleShot(
                    100, this,
                    [this]()
                    {
                        if (theoryEdit_ && theoryEdit_->verticalScrollBar())
                        {
                            QScrollBar *vBar = theoryEdit_->verticalScrollBar();
                            if (vBar->maximum() <= 0)
                            {
                                btnSubmit_->setEnabled(true);
                                btnSubmit_->setStyleSheet(
                                    "background-color: #2ecc71; color: white; font-weight: bold; border: none;");
                            }
                        }
                    });
            }
        }
    }

    if (btnNext_)
    {
        btnNext_->setEnabled(isCompleted);
        if (!isCompleted)
            btnNext_->setToolTip("Complete current task first");
        else
            btnNext_->setToolTip("");
    }
}

void TaskWindow::saveTaskProgress(bool success, const QString &code)
{
    if (currentUserId_ <= 0 || !userService_)
    {
        return;
    }

    if (currentModuleParentId_ == -1)
    {
        if (success)
        {
            if (btnSubmit_)
            {
                btnSubmit_->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
            }
            if (currentTask_.getId() == 9999)
            {
                emit customAchievementUnlocked("Easter Egg Finder");
            }
        }
        return;
    }

    bool saved = userService_->saveLessonProgress(currentUserId_, currentModuleParentId_, currentModuleId_, success);

    if (saved)
    {
        if (hasCodingTask_ && !code.isEmpty())
        {
            userService_->saveSubmission(currentUserId_, currentModuleParentId_, currentTask_.getId(), code, success);
        }

        if (success)
        {
            if (btnSubmit_)
            {
                btnSubmit_->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
            }
            if (btnNext_)
            {
                btnNext_->setEnabled(true);
                btnNext_->setToolTip("");
            }
            emit lessonCompleted(currentModuleId_);

            int totalProgress = getModuleProgress(currentModuleParentId_);
            emit moduleProgressUpdated(currentModuleParentId_, totalProgress);
        }
    }
    else
    {
        qDebug() << "Error in saveTaskProgress: failed to save to service";
    }
}

void TaskWindow::onNextTask()
{
    if (!courseService_)
    {
        return;
    }

    auto nextId = courseService_->getNextLessonId(currentModuleId_);
    if (nextId.has_value())
    {
        loadModule(nextId.value());
    }
}

void TaskWindow::onPrevTask()
{
    if (!courseService_)
    {
        return;
    }

    auto prevId = courseService_->getPrevLessonId(currentModuleId_);
    if (prevId.has_value())
    {
        loadModule(prevId.value());
    }
}

void TaskWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setMinimumSize(800, 580);
    resize(1200, 800);
    setObjectName("TaskWindow");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));

    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setIcon(QIcon(":/icons/main_logo.ico"));
    customTitleBar_->setTitle("Task");
    rootLayout->addWidget(customTitleBar_.get());

    QFrame *line = new QFrame();
    line->setFixedHeight(1);
    line->setStyleSheet("background-color: #999;");
    rootLayout->addWidget(line);

    auto mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(4);

    auto leftContainer = new QWidget();
    auto leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    auto tabHeader = new QFrame();
    tabHeader->setObjectName("tabHeader");
    tabHeader->setFixedHeight(60);
    auto tabLayout = new QHBoxLayout(tabHeader);
    auto btnTheory = new QPushButton("✧ Theory");
    auto btnPractice = new QPushButton("✧ Practice");
    btnTheory->setMinimumWidth(160);
    btnPractice->setMinimumWidth(160);
    btnPractice->setObjectName("tabButton");
    btnTheory->setObjectName("tabButton");
    btnPractice->setCheckable(true);
    btnTheory->setCheckable(true);
    btnTheory->setChecked(true);

    QFont tabFont("Roboto", 14, QFont::Bold);
    btnTheory->setFont(tabFont);
    btnPractice->setFont(tabFont);

    tabLayout->addWidget(btnTheory);
    tabLayout->addWidget(btnPractice);
    tabLayout->addStretch();
    leftLayout->addWidget(tabHeader);

    auto contentStack = new QStackedWidget();

    theoryEdit_ = new QTextEdit();
    theoryEdit_->setObjectName("theoryEdit");
    theoryEdit_->setReadOnly(true);
    theoryEdit_->setFrameStyle(QFrame::NoFrame);
    theoryEdit_->setFont(QFont("Roboto", 13));
    theoryEdit_->setStyleSheet("padding: 25px;");
    theoryEdit_->installEventFilter(this);

    practiceEdit_ = new QTextEdit();
    practiceEdit_->setObjectName("practiceEdit");
    practiceEdit_->setReadOnly(true);
    practiceEdit_->setFrameStyle(QFrame::NoFrame);
    practiceEdit_->setFont(QFont("Roboto", 13));
    practiceEdit_->setStyleSheet("padding: 25px;");
    practiceEdit_->installEventFilter(this);

    contentStack->addWidget(theoryEdit_);
    contentStack->addWidget(practiceEdit_);
    leftLayout->addWidget(contentStack);

    auto footerLeft = new QHBoxLayout();
    footerLeft->setContentsMargins(20, 20, 20, 20);

    btnBack_ = new QPushButton("←");
    btnBack_->setMinimumSize(50, 50);
    btnBack_->setMaximumSize(60, 60);
    btnBack_->setObjectName("backButton");
    footerLeft->addWidget(btnBack_);

    btnPrev_ = new QPushButton("Previous");
    btnNext_ = new QPushButton("Next");
    btnPrev_->setObjectName("navButton");
    btnNext_->setObjectName("navButton");
    btnPrev_->setMinimumSize(100, 50);
    btnPrev_->setMaximumSize(150, 60);
    btnNext_->setMinimumSize(100, 50);
    btnNext_->setMaximumSize(150, 60);

    footerLeft->addWidget(btnPrev_);
    footerLeft->addWidget(btnNext_);
    footerLeft->addStretch();
    leftLayout->addLayout(footerLeft);

    auto rightContainer = new QWidget();
    auto rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(20, 20, 20, 20);

    auto rightSplitter = new QSplitter(Qt::Vertical);
    auto codeFrame = new QFrame();
    codeFrame->setObjectName("editorFrame");
    auto codeLayout = new QVBoxLayout(codeFrame);

    codeLayout->addWidget(new QLabel("<\\> Code Editor"));

    codeEditor_ = new QTextEdit();
    codeEditor_->setObjectName("codeEditor");
    codeEditor_->setLineWrapMode(QTextEdit::WidgetWidth);
    codeEditor_->setFont(QFont("Consolas", 12));

    highlighter_ = std::make_unique<CppHighlighter>(codeEditor_->document());
    if (themeService_)
    {
        highlighter_->setTheme(themeService_->getCurrentTheme() == cppforge::services::Theme::Dark);
    }

    QFont codeFont("Consolas", 13);
    codeEditor_->setFont(codeFont);

    QFontMetrics metrics(codeFont);
    codeEditor_->setTabStopDistance(4 * metrics.horizontalAdvance(' '));

    codeEditor_->installEventFilter(this);
    codeLayout->addWidget(codeEditor_);

    auto codeActions = new QHBoxLayout();

    btnRun_ = new QPushButton("Run");
    btnSubmit_ = new QPushButton("Submit");

    btnRun_->setObjectName("runButton");
    btnSubmit_->setObjectName("submitButton");
    btnRun_->setMinimumSize(100, 45);
    btnRun_->setMaximumSize(140, 55);
    btnSubmit_->setMinimumSize(100, 45);
    btnSubmit_->setMaximumSize(140, 55);

    codeActions->addStretch();
    codeActions->addWidget(btnRun_);
    codeActions->addWidget(btnSubmit_);
    codeLayout->addLayout(codeActions);

    auto testFrame = new QFrame();
    testFrame->setObjectName("testFrame");
    auto testLayout = new QVBoxLayout(testFrame);
    testLayout->addWidget(new QLabel("✧ Test Result"));

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

    mainSplitter->addWidget(leftContainer);
    mainSplitter->addWidget(rightContainer);
    rootLayout->addWidget(mainSplitter);

    connect(btnTheory, &QPushButton::clicked,
            [=]()
            {
                contentStack->setCurrentIndex(0);
                btnTheory->setChecked(true);
                btnPractice->setChecked(false);
            });
    connect(btnPractice, &QPushButton::clicked,
            [=]()
            {
                contentStack->setCurrentIndex(1);
                btnPractice->setChecked(true);
                btnTheory->setChecked(false);
            });
    connect(btnBack_, &QPushButton::clicked, this, &TaskWindow::fadeOut);
    connect(btnNext_, &QPushButton::clicked, this, &TaskWindow::onNextTask);
    connect(btnPrev_, &QPushButton::clicked, this, &TaskWindow::onPrevTask);
    connect(btnRun_, &QPushButton::clicked, this, &TaskWindow::onRunClicked);
    connect(btnSubmit_, &QPushButton::clicked, this, &TaskWindow::onSubmitClicked);

    setupStyles();
}

void TaskWindow::onRunClicked()
{
    if (!hasCodingTask_ || !testOutput_)
    {
        return;
    }

    QString code = codeEditor_->toPlainText();
    testOutput_->clear();
    testOutput_->append("<b style='color:#3498db;'>[RUN]</b> Compiling and running...");

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
                        testOutput_->append("<span style='color:#e74c3c; font-weight:bold;'>Error:</span>");
                        testOutput_->append("<pre style='color:#c0392b; white-space: pre-wrap;'>" + result.getErrors() +
                                            "</pre>");
                    }
                }
                watcher->deleteLater();
            });

    watcher->setFuture(runner_->runAsync(code, runTests));
}

void TaskWindow::onSubmitClicked()
{
    if (currentUserId_ <= 0)
    {
        return;
    }

    if (!hasCodingTask_)
    {
        testOutput_->clear();
        testOutput_->append("<span style='color:#27ae60; font-weight:bold;'>[Success] Theory studied!</span>");
        saveTaskProgress(true, "");
        return;
    }

    QString code = codeEditor_->toPlainText();
    testOutput_->clear();
    testOutput_->append("<b style='color:#f39c12;'>[SUBMIT]</b> Full solution check...");

    auto violation = analyzer_->analyze(currentTask_, code);
    if (violation.has_value())
    {
        testOutput_->append("<span style='color:#e74c3c;'>[Analysis Error] " + violation.value() + "</span>");
        saveTaskProgress(false, code);
        return;
    }

    std::vector<cppforge::entities::TestCase> allTests(currentTask_.getTestCases().begin(),
                                                       currentTask_.getTestCases().end());

    auto watcher = new QFutureWatcher<cppforge::entities::ExecutionResult>(this);
    connect(watcher, &QFutureWatcher<cppforge::entities::ExecutionResult>::finished,
            [this, watcher, code]()
            {
                auto result = watcher->result();
                if (testOutput_)
                {
                    if (result.isSuccess())
                    {
                        testOutput_->append("<h3 style='color:#27ae60;'>✔ Task completed correctly!</h3>");
                        saveTaskProgress(true, code);
                    }
                    else
                    {
                        testOutput_->append("<h3 style='color:#e74c3c;'>✘ Solution not accepted</h3>");
                        testOutput_->append("<p>The program failed one or more tests.</p>");
                        if (!result.getErrors().isEmpty())
                        {
                            testOutput_->append("<pre style='color:red;'>" + result.getErrors() + "</pre>");
                        }
                        saveTaskProgress(false, code);
                    }
                }
                watcher->deleteLater();
            });

    watcher->setFuture(runner_->runAsync(code, allTests));
}

void TaskWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void TaskWindow::centerWindow()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QRect adj = screen->availableGeometry();
        move(adj.x() + (adj.width() - width()) / 2, adj.y() + (adj.height() - height()) / 2);
    }
}

bool TaskWindow::eventFilter(QObject *obj, QEvent *event)
{
    QTextEdit *editor = qobject_cast<QTextEdit *>(obj);
    if (!editor)
        return QWidget::eventFilter(obj, event);

    if (obj == codeEditor_ && event->type() == QEvent::KeyPress)
    {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab)
        {
            editor->insertPlainText("    ");
            return true;
        }

        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            QString currentLine = editor->textCursor().block().text();
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
                editor->insertPlainText("\n" + indent + "    ");
            }
            else
            {
                editor->insertPlainText("\n" + indent);
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
            editor->insertPlainText(QString(p.open) + p.close);
            editor->moveCursor(QTextCursor::Left);
            return true;
        }
    }

    if (obj == theoryEdit_ && !hasCodingTask_ && btnSubmit_ && !btnSubmit_->isEnabled())
    {
        if (event->type() == QEvent::Wheel || event->type() == QEvent::KeyPress)
        {
            QScrollBar *vBar = theoryEdit_->verticalScrollBar();
            if (vBar->value() >= vBar->maximum() - 20)
            {
                btnSubmit_->setEnabled(true);
                btnSubmit_->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
            }
        }
    }

    if (event->type() == QEvent::Wheel)
    {
        auto *wheelEvent = static_cast<QWheelEvent *>(event);
        if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier)
        {
            if (wheelEvent->angleDelta().y() > 0)
                editor->zoomIn(1);
            else
                editor->zoomOut(1);
            return true;
        }
    }

    if (event->type() == QEvent::KeyPress)
    {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->modifiers() & Qt::ControlModifier)
        {
            if (keyEvent->key() == Qt::Key_Plus || keyEvent->key() == Qt::Key_Equal)
            {
                editor->zoomIn(1);
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Minus)
            {
                editor->zoomOut(1);
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void TaskWindow::setupStyles(std::optional<bool> isDarkOverride)
{
    bool isDark = isDarkOverride.value_or(palette().color(QPalette::Window).lightness() < 128);

    QString accentColor = isDark ? "#0e639c" : "#62639b";
    QString hoverColor = isDark ? "#1177bb" : "#f3e8ff";
    QString hoverText = isDark ? "white" : "black";

    QString cssStyle =
        R"(
        #TaskWindow { background-color: palette(window); border: 1px solid palette(mid); border-radius: 20px; }
        #tabHeader { background-color: palette(alternate-base); border-bottom: 2px solid palette(mid); border-top-left-radius: 19px; border-top-right-radius: 19px; }
        
        QPushButton#tabButton { border: none; background: transparent; padding: 0 25px; color: palette(window-text); }
        QPushButton#tabButton:checked { border-bottom: 4px solid )" +
        accentColor + R"(; color: )" + accentColor + R"(; }
        
        QTextEdit { background-color: palette(base); color: palette(text); border: none; }

        QTextEdit#theoryEdit, QTextEdit#practiceEdit {
            padding: 30px;
        }
        QTextEdit#theoryEdit h1, QTextEdit#practiceEdit h1 { color: )" +
        accentColor +
        R"(; font-family: 'Outfit', 'Inter', sans-serif; font-size: 26px; font-weight: 800; margin-bottom: 15px; }
        QTextEdit#theoryEdit h2, QTextEdit#practiceEdit h2 { color: )" +
        accentColor +
        R"(; font-family: 'Outfit', 'Inter', sans-serif; font-size: 20px; font-weight: 700; border-left: 5px solid )" +
        accentColor + R"(; padding-left: 12px; margin-top: 25px; margin-bottom: 10px; }
        QTextEdit#theoryEdit h3, QTextEdit#practiceEdit h3 { color: )" +
        accentColor +
        R"(; font-family: 'Outfit', 'Inter', sans-serif; font-size: 18px; font-weight: 600; margin-top: 20px; }
        QTextEdit#theoryEdit p, QTextEdit#practiceEdit p { line-height: 1.6; margin-bottom: 12px; }
        QTextEdit#theoryEdit li, QTextEdit#practiceEdit li { margin-bottom: 8px; }
        QTextEdit#theoryEdit pre, QTextEdit#practiceEdit pre { background-color: palette(alternate-base); padding: 15px; border-radius: 10px; border: 1px solid palette(mid); font-family: 'Consolas', monospace; font-size: 13px; margin: 10px 0; }
        QTextEdit#theoryEdit code, QTextEdit#practiceEdit code { background-color: palette(alternate-base); padding: 2px 6px; border-radius: 4px; font-family: 'Consolas', monospace; font-weight: bold; }
        
        QPushButton#backButton {
            background-color: palette(alternate-base);
            color: palette(text);
            border-radius: 8px;
            font-size: 24px;
            border: 1px solid palette(mid);
        }
        QPushButton#backButton:hover { background-color: )" +
        hoverColor + R"( !important; color: )" + hoverText + R"( !important; border-color: )" + hoverColor +
        R"( !important; }
 
        QPushButton#navButton {
            background-color: palette(alternate-base);
            color: palette(text);
            border: 1px solid palette(mid);
            border-radius: 8px;
            font-weight: bold;
        }
        QPushButton#navButton:hover { background-color: )" +
        hoverColor + R"( !important; color: )" + hoverText + R"( !important; border-color: )" + hoverColor +
        R"( !important; }
        
        QFrame#editorFrame, QFrame#testFrame { background-color: palette(base); border: 1px solid palette(mid); border-radius: 12px; }
        
        QPushButton#runButton { 
            background-color: palette(base); 
            border: 1px solid palette(mid);
            border-radius: 6px; 
            font-weight: 600; 
            color: palette(text);
        }
        QPushButton#runButton:hover { background-color: )" +
        hoverColor + R"( !important; color: )" + hoverText + R"( !important; border-color: )" + hoverColor +
        R"( !important; }
        
        QPushButton#submitButton { 
            background-color: )" +
        accentColor + R"(; 
            border-radius: 6px; 
            font-weight: 600; 
            border: none; 
            color: white; 
        }
        QPushButton#submitButton:hover { background-color: )" +
        hoverColor + R"( !important; color: )" + hoverText + R"( !important; border-color: )" + hoverColor +
        R"( !important; }
        
        QLabel { color: palette(text); font-size: 14px; }
    )";

    setStyleSheet(cssStyle);

    style()->unpolish(this);
    style()->polish(this);
    update();
}

void TaskWindow::setThemeService(cppforge::services::ThemeService *service)
{
    themeService_ = service;
    if (themeService_)
    {
        connect(themeService_, &cppforge::services::ThemeService::themeChanged, this,
                [this](cppforge::services::Theme theme)
                {
                    if (customTitleBar_)
                    {
                        QString iconPath = (theme == cppforge::services::Theme::Dark) ? ":/icons/main_logo_dark.ico"
                                                                                      : ":/icons/main_logo.ico";
                        customTitleBar_->setIcon(QIcon(iconPath));
                        customTitleBar_->setThemeService(themeService_);
                    }
                    if (highlighter_)
                    {
                        highlighter_->setTheme(theme == cppforge::services::Theme::Dark);
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

void TaskWindow::fadeIn()
{
    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(300);
    transitionAnimation_->setStartValue(0.0);
    transitionAnimation_->setEndValue(1.0);
    transitionAnimation_->start();
}

void TaskWindow::fadeOut()
{
    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(250);
    transitionAnimation_->setStartValue(1.0);
    transitionAnimation_->setEndValue(0.0);
    connect(transitionAnimation_.get(), &QPropertyAnimation::finished, this,
            [this]()
            {
                WindowStateManager::instance().captureState(this);
                this->hide();
                emit windowClosed();
            });
    transitionAnimation_->start();
}

void TaskWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    fadeIn();
}

void TaskWindow::setUserId(int64_t id)
{
    if (id <= 0)
    {
        qWarning() << "Warning: TaskWindow received invalid User ID:" << id;
    }
    else
    {
        qDebug() << "TaskWindow: User ID set to" << id;
    }
    currentUserId_ = id;
}

int TaskWindow::getModuleProgress(int moduleId)
{
    if (!courseService_ || moduleId <= 0 || currentUserId_ <= 0)
    {
        return 0;
    }
    return courseService_->getModuleProgress(currentUserId_, moduleId);
}
