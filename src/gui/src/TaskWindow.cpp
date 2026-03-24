#include "../include/TaskWindow.hpp"
#include "../include/CppHighlighter.hpp"
#include "../include/CustomTitleBar.hpp"

#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QSplitter>
#include <QSqlError>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QStyleOption>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QPropertyAnimation>
#include <QFutureWatcher>

#include <optional>
#include <set>

TaskWindow::TaskWindow(QWidget *parent)
    : QWidget(parent), 
      runner_(std::make_unique<cppforge::services::CodeRunner>(this)),
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

    auto *practiceEdit = qobject_cast<QTextEdit *>(practiceContent_);
    if (practiceEdit)
        practiceEdit->setPlainText(task.getDescription());

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

    if (!query.exec()) {
        qDebug() << "КРИТИЧЕСКАЯ ОШИБКА SQL:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        QString title = query.value("title").toString();
        QString theory = query.value("content").toString();
        
        customTitleBar_->setTitle(title);
        
        auto* theoryEdit = qobject_cast<QTextEdit*>(theoryContent_);
        if (theoryEdit) {
            theoryEdit->setPlainText(theory);
        }

        QVariant taskIdVar = query.value("id");
        auto* practiceEdit = qobject_cast<QTextEdit*>(practiceContent_);
        
        if (!taskIdVar.isNull()) {
            uint64_t taskId = taskIdVar.toULongLong();
            QString practiceDesc = query.value("description").toString();
            QString initCode = query.value("initial_code").toString();
            int tLimit = query.value("time_limit").isValid() ? query.value("time_limit").toInt() : 1000;
            int mLimit = query.value("memory_limit").isValid() ? query.value("memory_limit").toInt() : 256;

            auto parseTags = [](const QString& str) -> std::optional<std::set<QString>> {
                if (str.trimmed().isEmpty()) return std::nullopt;
                std::set<QString> resultSet;
                QStringList list = str.split(',', Qt::SkipEmptyParts);
                for (const QString& item : list) resultSet.insert(item.trimmed());
                return resultSet;
            };

            std::optional<std::set<QString>> whitelist = parseTags(query.value("whitelist").toString());
            std::optional<std::set<QString>> blacklist = parseTags(query.value("blacklist").toString());

            std::set<cppforge::entities::TestCase> testCases;
            QSqlQuery testQuery;
            testQuery.prepare("SELECT id, input, expected_output, is_public FROM test_cases WHERE coding_task_id = :tid");
            testQuery.bindValue(":tid", taskId);

            if (testQuery.exec()) {
                while (testQuery.next()) {
                    testCases.emplace(
                        testQuery.value("id").toULongLong(),
                        testQuery.value("input").toString(),
                        testQuery.value("expected_output").toString(),
                        testQuery.value("is_public").toBool()
                    );
                }
            }

            currentTask_ = cppforge::entities::CodingTask(
                taskId, static_cast<uint64_t>(lessonId), title, practiceDesc, 
                initCode, testCases, tLimit, mLimit, whitelist, blacklist
            );

            if (practiceEdit) practiceEdit->setPlainText(practiceDesc);
            codeEditor_->setPlainText(initCode);
            codeEditor_->setReadOnly(false);
        } else {
            if (practiceEdit) practiceEdit->setPlainText("Для этого модуля практических заданий не предусмотрено.");
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
    if (query.exec() && query.next()) {
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
    if (query.exec() && query.next()) {
        loadModule(query.value(0).toInt());
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
    customTitleBar_->setTitle("Задание");
    customTitleBar_->setIcon(windowIcon());
    rootLayout->addWidget(customTitleBar_.get());

    QFrame *line = new QFrame();
    line->setFixedHeight(1);
    line->setStyleSheet("background-color: #999;");
    rootLayout->addWidget(line);

    auto mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(2);

    auto leftContainer = new QWidget();
    auto leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    auto tabHeader = new QFrame();
    tabHeader->setObjectName("tabHeader");
    auto tabLayout = new QHBoxLayout(tabHeader);
    auto btnTheory = new QPushButton("✧ Теория");
    auto btnPractice = new QPushButton("✧ Практика");
    btnPractice->setObjectName("tabButton");
    btnTheory->setObjectName("tabButton");
    btnPractice->setCheckable(true);
    btnTheory->setCheckable(true);
    
    btnTheory->setChecked(true);
    
    tabLayout->addWidget(btnTheory);
    tabLayout->addWidget(btnPractice);
    tabLayout->addStretch();
    leftLayout->addWidget(tabHeader);

    auto contentStack = new QStackedWidget();

    auto *theoryEdit = new QTextEdit();
    theoryEdit->setReadOnly(true);
    theoryEdit->setFrameStyle(QFrame::NoFrame);
    theoryEdit->setContentsMargins(20, 20, 20, 20);
    theoryContent_ = reinterpret_cast<QLabel *>(theoryEdit);

    auto *practiceEdit = new QTextEdit();
    practiceEdit->setReadOnly(true);
    practiceEdit->setFrameStyle(QFrame::NoFrame);
    practiceEdit->setContentsMargins(20, 20, 20, 20);
    practiceContent_ = reinterpret_cast<QLabel *>(practiceEdit);

    contentStack->addWidget(theoryEdit);    
    contentStack->addWidget(practiceEdit);  
    leftLayout->addWidget(contentStack);

    auto footerLeft = new QHBoxLayout();
    footerLeft->setContentsMargins(15, 15, 15, 15);
    
    auto btnBack = new QPushButton();
    btnBack->setFixedSize(45, 45);
    btnBack->setObjectName("backButton");
    footerLeft->addWidget(btnBack);

    footerLeft->addSpacing(10);
    
    auto btnPrev = new QPushButton("← Назад");
    auto btnNext = new QPushButton("Вперед →");
    btnPrev->setObjectName("navButton");
    btnNext->setObjectName("navButton");
    btnPrev->setFixedSize(100, 45);
    btnNext->setFixedSize(100, 45);
    
    footerLeft->addWidget(btnPrev);
    footerLeft->addWidget(btnNext);
    
    footerLeft->addStretch();
    leftLayout->addLayout(footerLeft);

    auto rightContainer = new QWidget();
    auto rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(15, 15, 15, 15);

    auto rightSplitter = new QSplitter(Qt::Vertical);
    auto codeFrame = new QFrame();
    codeFrame->setObjectName("editorFrame");
    auto codeLayout = new QVBoxLayout(codeFrame);
    codeLayout->addWidget(new QLabel("<\\> Code"));

    codeEditor_ = new QTextEdit();
    new CppHighlighter(codeEditor_->document());
    codeEditor_->setObjectName("codeEditor");
    codeEditor_->installEventFilter(this);

    QFont codeFont("Consolas", 12);
    codeFont.setStyleHint(QFont::Monospace);
    codeEditor_->setFont(codeFont);
    QFontMetrics metrics(codeFont);
    codeEditor_->setTabStopDistance(4 * metrics.horizontalAdvance(' '));

    codeLayout->addWidget(codeEditor_);

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
    testLayout->addWidget(new QLabel("✧ Test Result"));
    testOutput_ = new QTextEdit();
    testOutput_->setReadOnly(true);
    testOutput_->setObjectName("testOutput");
    testLayout->addWidget(testOutput_);

    rightSplitter->addWidget(codeFrame);
    rightSplitter->addWidget(testFrame);
    rightSplitter->setStretchFactor(0, 3);
    rightSplitter->setStretchFactor(1, 1);
    rightLayout->addWidget(rightSplitter);

    mainSplitter->addWidget(leftContainer);
    mainSplitter->addWidget(rightContainer);
    rootLayout->addWidget(mainSplitter);

    connect(btnTheory, &QPushButton::clicked, [=]() {
        contentStack->setCurrentIndex(0);
        btnTheory->setChecked(true);
        btnPractice->setChecked(false);
    });
    connect(btnPractice, &QPushButton::clicked, [=]() {
        contentStack->setCurrentIndex(1);
        btnPractice->setChecked(true);
        btnTheory->setChecked(false);
    });
    
    connect(btnBack, &QPushButton::clicked, this, &TaskWindow::fadeOut);
    connect(btnNext, &QPushButton::clicked, this, &TaskWindow::onNextTask);
    connect(btnPrev, &QPushButton::clicked, this, &TaskWindow::onPrevTask);
    connect(btnRun, &QPushButton::clicked, this, &TaskWindow::onRunClicked);
    connect(btnSubmit, &QPushButton::clicked, this, &TaskWindow::onSubmitClicked);

    setupStyles();
}

void TaskWindow::onRunClicked()
{
    QString code = codeEditor_->toPlainText();
    testOutput_->clear();
    
    if (currentTask_.getId() == 0) {
        testOutput_->append("<span style='color:orange;'>Это теоретический модуль. Практика недоступна.</span>");
        return;
    }

    testOutput_->append("Анализ безопасности...");
    auto violation = analyzer_->analyze(currentTask_, code);
    if (violation.has_value()) {
        testOutput_->append("<span style='color:red;'>Ошибка: " + violation.value() + "</span>");
        return;
    }

    testOutput_->append("Компиляция...");
    std::vector<cppforge::entities::TestCase> testVector(currentTask_.getTestCases().begin(), currentTask_.getTestCases().end());

    auto watcher = new QFutureWatcher<cppforge::entities::ExecutionResult>(this);
    connect(watcher, &QFutureWatcher<cppforge::entities::ExecutionResult>::finished, [this, watcher]() {
        auto result = watcher->result();
        if (result.isSuccess()) {
            testOutput_->append("<span style='color:green;'>[OK] Все тесты пройдены!</span>");
            testOutput_->append("Вывод:\n" + result.getOutput());
        } else {
            testOutput_->append("<span style='color:red;'>[FAIL] Ошибка выполнения.</span>");
            if (!result.getErrors().isEmpty()) testOutput_->append(result.getErrors());
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
    if (screen) {
        QRect adj = screen->availableGeometry();
        move(adj.x() + (adj.width() - width()) / 2, adj.y() + (adj.height() - height()) / 2);
    }
}

bool TaskWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == codeEditor_ && event->type() == QEvent::Wheel) {
        auto *wheelEvent = static_cast<QWheelEvent *>(event);
        if (wheelEvent->modifiers() & Qt::ControlModifier) {
            if (wheelEvent->angleDelta().y() > 0) codeEditor_->zoomIn(1);
            else codeEditor_->zoomOut(1);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void TaskWindow::setupStyles()
{
    setStyleSheet(R"(
        #TaskWindow { background-color: white; border: 1px solid #777; }
        #tabHeader { background-color: #eeeeee; border-bottom: 1px solid #bbbbbb; min-height: 40px; }
        QPushButton#tabButton { border: none; background: transparent; font-weight: bold; font-size: 13px; padding: 5px 10px; }
        QPushButton#tabButton:checked { border-bottom: 2px solid black; }
        #editorFrame, #testFrame { background-color: white; border: 2px solid #dddddd; border-radius: 0px; }
        #codeEditor, #testOutput { border: none; font-family: 'Consolas';}
        QPushButton#runButton, QPushButton#submitButton, QPushButton#navButton { border-radius: 0px; padding: 5px 20px; font-weight: bold; border: 1px solid #ccc; }
        QPushButton#runButton, QPushButton#navButton { background-color: #f0f0f0; }
        QPushButton#submitButton { background-color: #b8e2c8; border: none; }
        QPushButton#backButton { background-color: #d9d9d9; border-radius: 0px; border: none; }
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
    transitionAnimation_->setDuration(200);
    transitionAnimation_->setStartValue(1.0);
    transitionAnimation_->setEndValue(0.0);
    connect(transitionAnimation_.get(), &QPropertyAnimation::finished, this, [this]() {
        hide();
        emit windowClosed();
    });
    transitionAnimation_->start();
}

void TaskWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    fadeIn();
}