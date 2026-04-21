#include "../include/TaskWindow.hpp"

#include "../include/CppHighlighter.hpp"

#include "../include/CustomTitleBar.hpp"

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

    if (testOutput_)
        testOutput_->clear();

    if (btnSubmit_)
    {
        btnSubmit_->setEnabled(true);
        btnSubmit_->setText("Submit");
        btnSubmit_->setStyleSheet("");
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT 
            l.title, 
            l.content, 
            l.module_id,
            t.id AS task_id, 
            t.description, 
            t.initial_code, 
            t.whitelist, 
            t.blacklist,
            (SELECT is_completed FROM user_progress WHERE user_id = :uid AND lesson_id = l.id) as is_done
        FROM lessons l 
        LEFT JOIN coding_tasks t ON l.id = t.lesson_id 
        WHERE l.id = :lessonId
    )");
    query.bindValue(":lessonId", lessonId);
    query.bindValue(":uid", static_cast<qlonglong>(currentUserId_));

    if (!query.exec() || !query.next())
        return;

    QString title = query.value("title").toString();
    QString theory = query.value("content").toString();
    bool isCompleted = query.value("is_done").toBool();
    currentModuleParentId_ = query.value("module_id").toInt();

    customTitleBar_->setTitle(title);
    if (theoryEdit_)
    {
        theoryEdit_->setPlainText(theory);
        applyTextFormatting(theoryEdit_);
    }

    QVariant taskIdVar = query.value("task_id");

    if (!taskIdVar.isNull())
    {
        hasCodingTask_ = true;
        uint64_t taskId = taskIdVar.toULongLong();
        QString practiceDesc = query.value("description").toString();
        QString initCode = query.value("initial_code").toString();

        if (btnRun_)
            btnRun_->setVisible(true);
        if (practiceEdit_)
        {
            practiceEdit_->setPlainText(practiceDesc);
            applyTextFormatting(practiceEdit_);
        }

        QSqlQuery loadSubQuery;
        loadSubQuery.prepare("SELECT source_code FROM submissions WHERE user_id = :uid AND coding_task_id = :tid ORDER "
                             "BY submitted_at DESC LIMIT 1");
        loadSubQuery.bindValue(":uid", static_cast<qlonglong>(currentUserId_));
        loadSubQuery.bindValue(":tid", static_cast<qulonglong>(taskId));

        if (loadSubQuery.exec() && loadSubQuery.next())
        {
            codeEditor_->setPlainText(loadSubQuery.value(0).toString());
        }
        else
        {
            codeEditor_->setPlainText(initCode);
        }

        if (isCompleted && testOutput_)
        {
            testOutput_->append(
                "<span style='color:#27ae60; font-weight:bold;'>[Статус] Это задание уже было выполнено верно.</span>");
            if (btnSubmit_)
                btnSubmit_->setStyleSheet("background-color: #b8e2c8; color: #2d5a3d; font-weight: bold;");
        }

        codeEditor_->setReadOnly(false);

        std::set<cppforge::entities::TestCase> testCases;
        QSqlQuery testQuery;
        testQuery.prepare("SELECT id, input, expected_output, is_public FROM test_cases WHERE coding_task_id = :tid");
        testQuery.bindValue(":tid", static_cast<qulonglong>(taskId));
        if (testQuery.exec())
        {
            while (testQuery.next())
            {
                testCases.emplace(testQuery.value("id").toULongLong(), testQuery.value("input").toString(),
                                  testQuery.value("expected_output").toString(), testQuery.value("is_public").toBool());
            }
        }
        currentTask_ = cppforge::entities::CodingTask(taskId, static_cast<uint64_t>(lessonId), title, practiceDesc,
                                                      initCode, testCases, 1000, 256, std::nullopt, std::nullopt);
    }
    else
    {
        hasCodingTask_ = false;
        if (practiceEdit_)
        {
            practiceEdit_->setPlainText("Для этого модуля практических заданий не предусмотрено.");
            applyTextFormatting(practiceEdit_);
        }
        codeEditor_->setPlainText("// Только теоретический материал.");
        codeEditor_->setReadOnly(true);
        if (btnRun_)
            btnRun_->setVisible(false);

        if (btnSubmit_)
        {
            btnSubmit_->setText("Изучено");
            if (isCompleted)
            {
                if (testOutput_)
                    testOutput_->append(
                        "<span style='color:#27ae60; font-weight:bold;'>[Статус] Теория изучена.</span>");
                btnSubmit_->setEnabled(true);
                btnSubmit_->setStyleSheet("background-color: #b8e2c8; color: #2d5a3d; font-weight: bold;");
            }
            else
            {
                btnSubmit_->setEnabled(false);
                btnSubmit_->setStyleSheet("background-color: #f0f0f0; color: #888;");
            }
        }
    }

    if (btnNext_)
    {
        btnNext_->setEnabled(isCompleted);
        if (!isCompleted)
            btnNext_->setToolTip("Сначала завершите текущее задание");
        else
            btnNext_->setToolTip("");
    }
}

void TaskWindow::onNextTask()
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT id FROM lessons 
        WHERE (module_id, order_index) > (
            SELECT module_id, order_index FROM lessons WHERE id = :id
        )
        ORDER BY module_id ASC, order_index ASC LIMIT 1
    )");
    query.bindValue(":id", currentModuleId_);
    if (query.exec() && query.next())
        loadModule(query.value(0).toInt());
}

void TaskWindow::onPrevTask()
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT id FROM lessons 
        WHERE (module_id, order_index) < (
            SELECT module_id, order_index FROM lessons WHERE id = :id
        )
        ORDER BY module_id DESC, order_index DESC LIMIT 1
    )");
    query.bindValue(":id", currentModuleId_);
    if (query.exec() && query.next())
        loadModule(query.value(0).toInt());
}

void TaskWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setFixedSize(1300, 900);
    setObjectName("TaskWindow");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));

    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setIcon(QIcon(":/icons/main_logo.ico"));
    customTitleBar_->setTitle("Задание");
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
    theoryEdit_->setStyleSheet("padding: 25px;");
    theoryEdit_->installEventFilter(this);

    practiceEdit_ = new QTextEdit();
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
    btnBack_->setFixedSize(55, 55);
    btnBack_->setObjectName("backButton");
    footerLeft->addWidget(btnBack_);

    btnPrev_ = new QPushButton("Назад");
    btnNext_ = new QPushButton("Вперед");
    btnPrev_->setObjectName("navButton");
    btnNext_->setObjectName("navButton");
    btnPrev_->setFixedSize(130, 55);
    btnNext_->setFixedSize(130, 55);

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
    new CppHighlighter(codeEditor_->document());
    codeEditor_->setObjectName("codeEditor");

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
    btnRun_->setFixedSize(120, 50);
    btnSubmit_->setFixedSize(120, 50);

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
    if (currentTask_.getId() == 0 || !testOutput_)
        return;

    QString code = codeEditor_->toPlainText();
    testOutput_->clear();
    testOutput_->append("Анализ...");

    auto violation = analyzer_->analyze(currentTask_, code);
    if (violation.has_value())
    {
        testOutput_->append("<span style='color:red;'>Ошибка: " + violation.value() + "</span>");
        return;
    }

    testOutput_->append("Запуск...");
    std::vector<cppforge::entities::TestCase> testVector(currentTask_.getTestCases().begin(),
                                                         currentTask_.getTestCases().end());

    auto watcher = new QFutureWatcher<cppforge::entities::ExecutionResult>(this);
    connect(watcher, &QFutureWatcher<cppforge::entities::ExecutionResult>::finished,
            [this, watcher]()
            {
                auto result = watcher->result();
                if (testOutput_)
                {
                    if (result.isSuccess())
                    {
                        testOutput_->append("<span style='color:green; font-weight:bold;'>[OK] Успешно!</span>");
                    }
                    else
                    {
                        testOutput_->append("<span style='color:red;'>[FAIL] " + result.getErrors() + "</span>");
                    }
                }
                watcher->deleteLater();
            });

    watcher->setFuture(runner_->runAsync(code, testVector));
}

void TaskWindow::onSubmitClicked()
{
    if (currentUserId_ <= 0)
    {
        if (testOutput_)
        {
            testOutput_->append(
                "<span style='color:red;'>[Error] Не удалось сохранить прогресс: пользователь не авторизован.</span>");
        }
        return;
    }

    if (!testOutput_)
        return;

    auto saveProgress = [this](bool isPractice, const QString &code = "", bool success = false)
    {
        QSqlQuery query;
        query.prepare(R"(
            INSERT INTO user_progress (user_id, module_id, lesson_id, is_completed, updated_at) 
            VALUES (:uid, :mid, :lid, :status, CURRENT_TIMESTAMP) 
            ON CONFLICT (user_id, lesson_id) 
            DO UPDATE SET 
                is_completed = EXCLUDED.is_completed, 
                updated_at = CURRENT_TIMESTAMP
        )");
        query.bindValue(":uid", static_cast<qlonglong>(currentUserId_));
        query.bindValue(":mid", static_cast<qlonglong>(currentModuleParentId_));
        query.bindValue(":lid", static_cast<qlonglong>(currentModuleId_));
        query.bindValue(":status", !isPractice || success);

        if (query.exec())
        {
            if (isPractice && !code.isEmpty())
            {
                QSqlQuery subQuery;
                subQuery.prepare(R"(
                    INSERT INTO submissions (user_id, module_id, coding_task_id, source_code, is_success, submitted_at) 
                    VALUES (:uid, :mid, :tid, :code, :success, CURRENT_TIMESTAMP)
                )");
                subQuery.bindValue(":uid", static_cast<qlonglong>(currentUserId_));
                subQuery.bindValue(":mid", static_cast<qlonglong>(currentModuleParentId_));
                subQuery.bindValue(":tid", static_cast<qlonglong>(currentTask_.getId()));
                subQuery.bindValue(":code", code);
                subQuery.bindValue(":success", success);
                subQuery.exec();
            }

            if (!isPractice || success)
            {
                if (btnSubmit_)
                {
                    btnSubmit_->setStyleSheet("background-color: #b8e2c8; color: #2d5a3d; font-weight: bold;");
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
            qDebug() << "SQL Error in saveProgress:" << query.lastError().text();
        }
    };

    if (!hasCodingTask_)
    {
        testOutput_->clear();
        testOutput_->append("<span style='color:#27ae60; font-weight:bold;'>[Успех] Теория изучена!</span>");
        saveProgress(false);
    }
    else
    {
        QString code = codeEditor_->toPlainText();
        testOutput_->clear();
        testOutput_->append("Проверка решения...");

        auto violation = analyzer_->analyze(currentTask_, code);
        if (violation.has_value())
        {
            testOutput_->append("<span style='color:#e74c3c;'>[Ошибка анализа] " + violation.value() + "</span>");
            saveProgress(true, code, false);
            return;
        }

        std::vector<cppforge::entities::TestCase> testVector(currentTask_.getTestCases().begin(),
                                                             currentTask_.getTestCases().end());

        auto watcher = new QFutureWatcher<cppforge::entities::ExecutionResult>(this);

        connect(watcher, &QFutureWatcher<cppforge::entities::ExecutionResult>::finished,
                [this, watcher, code, saveProgress]()
                {
                    auto result = watcher->result();
                    if (testOutput_)
                    {
                        if (result.isSuccess())
                        {
                            testOutput_->append(
                                "<span style='color:#27ae60; font-weight:bold;'>[Успех] Все тесты пройдены!</span>");
                            saveProgress(true, code, true);
                        }
                        else
                        {
                            testOutput_->append("<span style='color:#e74c3c;'>[Ошибка] Тесты не пройдены:</span>");
                            testOutput_->append("<pre>" + result.getErrors() + "</pre>");
                            saveProgress(true, code, false);
                        }
                    }
                    watcher->deleteLater();
                });

        watcher->setFuture(runner_->runAsync(code, testVector));
    }
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
    }

    if (obj == theoryEdit_ && !hasCodingTask_ && btnSubmit_ && !btnSubmit_->isEnabled())
    {
        if (event->type() == QEvent::Wheel || event->type() == QEvent::KeyPress)
        {
            QScrollBar *vBar = theoryEdit_->verticalScrollBar();
            if (vBar->value() >= vBar->maximum() - 20)
            {
                btnSubmit_->setEnabled(true);
                btnSubmit_->setStyleSheet("background-color: #b8e2c8; color: #2d5a3d; font-weight: bold;");
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
        QPushButton#submitButton { background-color: #b8e2c8; border: none; color: #2d5a3d; }
        QPushButton#backButton { background-color: #e0e0e0; border-radius: 8px; border: none; color: #444; }
    )");
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
    if (moduleId <= 0)
        return 0;

    QSqlQuery query;
    query.prepare(R"(
        SELECT 
            (SELECT COUNT(*) FROM lessons WHERE module_id = :mid) as total,
            (SELECT COUNT(*) FROM user_progress 
             WHERE module_id = :mid AND user_id = :uid AND is_completed = true) as completed
    )");
    query.bindValue(":mid", moduleId);
    query.bindValue(":uid", static_cast<qlonglong>(currentUserId_));

    if (query.exec() && query.next())
    {
        int total = query.value("total").toInt();
        int completed = query.value("completed").toInt();

        if (total <= 0)
            return 0;

        int progress = static_cast<int>((static_cast<double>(completed) / total) * 100.0);
        return std::clamp(progress, 0, 100);
    }

    qDebug() << "Progress SQL Error:" << query.lastError().text();
    return 0;
}