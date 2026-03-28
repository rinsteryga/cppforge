#include "../include/TaskWindow.hpp"

#include "../include/CppHighlighter.hpp"
#include "../include/CustomTitleBar.hpp"

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

// Функция для интеграции "темы с абзацами"
void applyTextFormatting(QTextEdit *editor)
{
    if (!editor)
        return;
    QTextCursor cursor = editor->textCursor();
    QTextBlockFormat blockFormat;
    blockFormat.setBottomMargin(15);                                      // Отступ между абзацами
    blockFormat.setLineHeight(145, QTextBlockFormat::ProportionalHeight); // Межстрочный интервал
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
    customTitleBar_->setTitle(task.getTitle());

    if (practiceEdit_)
    {
        practiceEdit_->setPlainText(task.getDescription());
        applyTextFormatting(practiceEdit_);
    }

    codeEditor_->setPlainText(task.getInitialCode());
}

void TaskWindow::loadModule(int lessonId)
{
    currentModuleId_ = lessonId;
    qDebug() << "TaskWindow: Начало загрузки урока ID:" << lessonId;

    testOutput_->clear();

    QSqlQuery query;
    query.prepare(R"(
        SELECT 
            l.title, 
            l.content, 
            t.id, 
            t.description, 
            t.initial_code, 
            t.whitelist, 
            t.blacklist, 
            t.time_limit, 
            t.memory_limit 
        FROM lessons l 
        LEFT JOIN coding_tasks t ON l.id = t.lesson_id 
        WHERE l.id = :lessonId
    )");
    query.bindValue(":lessonId", lessonId);

    if (!query.exec())
    {
        qDebug() << "КРИТИЧЕСКАЯ ОШИБКА SQL:" << query.lastError().text();
        return;
    }

    if (query.next())
    {
        QString title = query.value("title").toString();
        QString theory = query.value("content").toString();

        customTitleBar_->setTitle(title);

        if (theoryEdit_)
        {
            theoryEdit_->setPlainText(theory);
            applyTextFormatting(theoryEdit_);
        }

        QVariant taskIdVar = query.value("id");

        if (!taskIdVar.isNull())
        {
            uint64_t taskId = taskIdVar.toULongLong();
            QString practiceDesc = query.value("description").toString();
            QString initCode = query.value("initial_code").toString();
            int tLimit = query.value("time_limit").isValid() ? query.value("time_limit").toInt() : 1000;
            int mLimit = query.value("memory_limit").isValid() ? query.value("memory_limit").toInt() : 256;

            auto parseTags = [](const QString &str) -> std::optional<std::set<QString>>
            {
                if (str.trimmed().isEmpty())
                    return std::nullopt;
                std::set<QString> resultSet;
                QStringList list = str.split(',', Qt::SkipEmptyParts);
                for (const QString &item : list)
                    resultSet.insert(item.trimmed());
                return resultSet;
            };

            std::optional<std::set<QString>> whitelist = parseTags(query.value("whitelist").toString());
            std::optional<std::set<QString>> blacklist = parseTags(query.value("blacklist").toString());

            std::set<cppforge::entities::TestCase> testCases;
            QSqlQuery testQuery;
            testQuery.prepare(
                "SELECT id, input, expected_output, is_public FROM test_cases WHERE coding_task_id = :tid");
            testQuery.bindValue(":tid", static_cast<qulonglong>(taskId));

            if (testQuery.exec())
            {
                while (testQuery.next())
                {
                    testCases.emplace(testQuery.value("id").toULongLong(), testQuery.value("input").toString(),
                                      testQuery.value("expected_output").toString(),
                                      testQuery.value("is_public").toBool());
                }
            }

            currentTask_ = cppforge::entities::CodingTask(taskId, static_cast<uint64_t>(lessonId), title, practiceDesc,
                                                          initCode, testCases, tLimit, mLimit, whitelist, blacklist);

            if (practiceEdit_)
            {
                practiceEdit_->setPlainText(practiceDesc);
                applyTextFormatting(practiceEdit_);
            }
            codeEditor_->setPlainText(initCode);
            codeEditor_->setReadOnly(false);
        }
        else
        {
            if (practiceEdit_)
            {
                practiceEdit_->setPlainText("Для этого модуля практических заданий не предусмотрено.");
                applyTextFormatting(practiceEdit_);
            }
            codeEditor_->setPlainText("// В этом уроке только теоретический материал.");
            codeEditor_->setReadOnly(true);
            currentTask_ = cppforge::entities::CodingTask();
        }
    }
}

void TaskWindow::onNextTask()
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT id FROM lessons 
        WHERE order_index > (SELECT order_index FROM lessons WHERE id = :id) 
        ORDER BY order_index ASC LIMIT 1
    )");
    query.bindValue(":id", currentModuleId_);
    if (query.exec() && query.next())
    {
        loadModule(query.value(0).toInt());
    }
}

void TaskWindow::onPrevTask()
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT id FROM lessons 
        WHERE order_index < (SELECT order_index FROM lessons WHERE id = :id) 
        ORDER BY order_index DESC LIMIT 1
    )");
    query.bindValue(":id", currentModuleId_);
    if (query.exec() && query.next())
    {
        loadModule(query.value(0).toInt());
    }
}

void TaskWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setFixedSize(1300, 900);
    setObjectName("TaskWindow");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));

    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setTitle("Задание");
    customTitleBar_->setIcon(windowIcon());
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
    auto btnTheory = new QPushButton("✧ Теория");
    auto btnPractice = new QPushButton("✧ Практика");
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
    theoryEdit_->setReadOnly(true);
    theoryEdit_->setFrameStyle(QFrame::NoFrame);
    theoryEdit_->setFont(QFont("Roboto", 13));
    theoryEdit_->setStyleSheet("padding: 25px; line-height: 1.6;");
    theoryEdit_->installEventFilter(this); // УСТАНОВКА ФИЛЬТРА

    practiceEdit_ = new QTextEdit();
    practiceEdit_->setReadOnly(true);
    practiceEdit_->setFrameStyle(QFrame::NoFrame);
    practiceEdit_->setFont(QFont("Roboto", 13));
    practiceEdit_->setStyleSheet("padding: 25px;");
    practiceEdit_->installEventFilter(this); // УСТАНОВКА ФИЛЬТРА

    contentStack->addWidget(theoryEdit_);
    contentStack->addWidget(practiceEdit_);
    leftLayout->addWidget(contentStack);

    auto footerLeft = new QHBoxLayout();
    footerLeft->setContentsMargins(20, 20, 20, 20);

    btnBack_ = new QPushButton("←");
    btnBack_->setFixedSize(55, 55);
    btnBack_->setObjectName("backButton");
    btnBack_->setFont(QFont("Roboto", 18, QFont::Bold));
    footerLeft->addWidget(btnBack_);

    footerLeft->addSpacing(15);

    btnPrev_ = new QPushButton("Назад");
    btnNext_ = new QPushButton("Вперед");
    btnPrev_->setObjectName("navButton");
    btnNext_->setObjectName("navButton");
    btnPrev_->setFixedSize(130, 55);
    btnNext_->setFixedSize(130, 55);

    QFont navFont("Roboto", 12, QFont::Bold);
    btnPrev_->setFont(navFont);
    btnNext_->setFont(navFont);

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

    auto codeLabel = new QLabel("<\\> Code Editor");
    codeLabel->setFont(QFont("Roboto", 12, QFont::Bold));
    codeLayout->addWidget(codeLabel);

    codeEditor_ = new QTextEdit();
    new CppHighlighter(codeEditor_->document());
    codeEditor_->setObjectName("codeEditor");
    codeEditor_->installEventFilter(this); // УСТАНОВКА ФИЛЬТРА

    QFont codeFont("Consolas", 13);
    codeEditor_->setFont(codeFont);
    QFontMetrics metrics(codeFont);
    codeEditor_->setTabStopDistance(4 * metrics.horizontalAdvance(' '));

    codeLayout->addWidget(codeEditor_);

    auto codeActions = new QHBoxLayout();
    auto btnRun = new QPushButton("Run");
    auto btnSubmit = new QPushButton("Submit");
    btnRun->setObjectName("runButton");
    btnSubmit->setObjectName("submitButton");
    btnRun->setFixedSize(120, 50);
    btnSubmit->setFixedSize(120, 50);
    btnRun->setFont(navFont);
    btnSubmit->setFont(navFont);

    codeActions->addStretch();
    codeActions->addWidget(btnRun);
    codeActions->addWidget(btnSubmit);
    codeLayout->addLayout(codeActions);

    auto testFrame = new QFrame();
    testFrame->setObjectName("testFrame");
    auto testLayout = new QVBoxLayout(testFrame);
    auto testLabel = new QLabel("✧ Test Result");
    testLabel->setFont(QFont("Roboto", 12, QFont::Bold));
    testLayout->addWidget(testLabel);

    testOutput_ = new QTextEdit();
    testOutput_->setReadOnly(true);
    testOutput_->setObjectName("testOutput");
    testOutput_->setFont(QFont("Consolas", 12));
    testOutput_->installEventFilter(this); // Опционально: зум и в выводе тестов
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
    connect(btnRun, &QPushButton::clicked, this, &TaskWindow::onRunClicked);
    connect(btnSubmit, &QPushButton::clicked, this, &TaskWindow::onSubmitClicked);

    setupStyles();
}

void TaskWindow::onRunClicked()
{
    QString code = codeEditor_->toPlainText();
    testOutput_->clear();

    if (currentTask_.getId() == 0)
    {
        testOutput_->append("<span style='color:orange;'>Это теоретический модуль. Практика недоступна.</span>");
        return;
    }

    testOutput_->append("Анализ безопасности...");
    auto violation = analyzer_->analyze(currentTask_, code);
    if (violation.has_value())
    {
        testOutput_->append("<span style='color:red;'>Ошибка: " + violation.value() + "</span>");
        return;
    }

    testOutput_->append("Компиляция...");
    std::vector<cppforge::entities::TestCase> testVector(currentTask_.getTestCases().begin(),
                                                         currentTask_.getTestCases().end());

    auto watcher = new QFutureWatcher<cppforge::entities::ExecutionResult>(this);
    connect(watcher, &QFutureWatcher<cppforge::entities::ExecutionResult>::finished,
            [this, watcher]()
            {
                auto result = watcher->result();
                if (result.isSuccess())
                {
                    testOutput_->append("<span style='color:green; font-weight:bold;'>[OK] Все тесты пройдены!</span>");
                    testOutput_->append("Вывод:\n" + result.getOutput());
                }
                else
                {
                    testOutput_->append("<span style='color:red; font-weight:bold;'>[FAIL] Ошибка выполнения.</span>");
                    if (!result.getErrors().isEmpty())
                        testOutput_->append("<pre style='color:#ff4444;'>" + result.getErrors() + "</pre>");
                }
                watcher->deleteLater();
            });

    watcher->setFuture(runner_->runAsync(code, testVector));
}

void TaskWindow::onSubmitClicked()
{
    onRunClicked();
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

// УНИВЕРСАЛЬНЫЙ ФИЛЬТР СОБЫТИЙ ДЛЯ ЗУМА
bool TaskWindow::eventFilter(QObject *obj, QEvent *event)
{
    QTextEdit *editor = qobject_cast<QTextEdit *>(obj);
    if (editor)
    {
        if (event->type() == QEvent::Wheel)
        {
            auto *wheelEvent = static_cast<QWheelEvent *>(event);
            if (wheelEvent->modifiers() & Qt::ControlModifier)
            {
                if (wheelEvent->angleDelta().y() > 0)
                    editor->zoomIn(1);
                else
                    editor->zoomOut(1);
                return true;
            }
        }
        else if (event->type() == QEvent::KeyPress)
        {
            auto *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->modifiers() & Qt::ControlModifier)
            {
                if (keyEvent->key() == Qt::Key_Plus || keyEvent->key() == Qt::Key_Equal)
                {
                    editor->zoomIn(1);
                    return true;
                }
                if (keyEvent->key() == Qt::Key_Minus)
                {
                    editor->zoomOut(1);
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void TaskWindow::setupStyles()
{
    setStyleSheet(R"(
        #TaskWindow { background-color: white; border: 1px solid #777; }
        #tabHeader { background-color: #f8f8f8; border-bottom: 2px solid #ddd; }
        QPushButton#tabButton { border: none; background: transparent; padding: 0 25px; color: #666; }
        QPushButton#tabButton:checked { border-bottom: 4px solid #62639b; color: #62639b; }
        #editorFrame, #testFrame { background-color: white; border: 1px solid #ccc; border-radius: 8px; }
        #codeEditor, #testOutput { border: none; }
        QPushButton#runButton, QPushButton#submitButton, QPushButton#navButton { 
            border-radius: 8px; font-weight: bold; border: 1px solid #ccc; 
        }
        QPushButton#runButton, QPushButton#navButton { background-color: #f0f0f0; }
        QPushButton#runButton:hover { background-color: #e5e5e5; }
        QPushButton#submitButton { background-color: #b8e2c8; border: none; color: #2d5a3d; }
        QPushButton#submitButton:hover { background-color: #a4cfb5; }
        QPushButton#backButton { background-color: #e0e0e0; border-radius: 8px; border: none; color: #444; }
        QPushButton#backButton:hover { background-color: #d5d5d5; }
    )");
}

void TaskWindow::fadeIn()
{
    if (transitionAnimation_ && transitionAnimation_->state() == QAbstractAnimation::Running)
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
    if (transitionAnimation_ && transitionAnimation_->state() == QAbstractAnimation::Running)
        transitionAnimation_->stop();

    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(250);
    transitionAnimation_->setStartValue(1.0);
    transitionAnimation_->setEndValue(0.0);
    transitionAnimation_->setEasingCurve(QEasingCurve::InOutCubic);

    connect(transitionAnimation_.get(), &QPropertyAnimation::finished, this,
            [this]()
            {
                this->hide();
                if (parentWidget())
                {
                    parentWidget()->show();
                }
                emit windowClosed();
            });
    transitionAnimation_->start();
}

void TaskWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    fadeIn();
}