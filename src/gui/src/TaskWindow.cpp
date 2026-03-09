#include "TaskWindow.hpp"

#include "TaskManager.hpp"

#include <QCloseEvent>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

TaskWindow::TaskWindow(QWidget *parent)
    : QWidget(parent), taskManager_(nullptr), currentModuleId_(-1), currentTaskId_(-1), currentTheoryIndex_(-1),
      tabWidget_(nullptr), theoryTab_(nullptr), practiceTab_(nullptr), theorySelector_(nullptr),
      theoryDisplay_(nullptr), prevTheoryButton_(nullptr), nextTheoryButton_(nullptr), theoryPageLabel_(nullptr),
      practiceSplitter_(nullptr), taskPanel_(nullptr), taskTitle_(nullptr), taskDescription_(nullptr),
      taskSelector_(nullptr), codeEditor_(nullptr), inputField_(nullptr), outputField_(nullptr), runButton_(nullptr),
      checkButton_(nullptr), resultLabel_(nullptr)
{
    setupUI();
    setupConnections();
    setWindowOpacity(0.0);

    taskManager_ = &TaskManager::instance();
    codeRunner_ = std::make_unique<cppforge::services::CodeRunner>(this);
    analyzer_ = std::make_unique<cppforge::services::StaticAnalyzer>();
    taskManager_->loadTasks(":/tasks/tasks.json");
}

TaskWindow::~TaskWindow()
{
    saveCurrentCode();
}

void TaskWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    fadeIn();
}

void TaskWindow::closeEvent(QCloseEvent *event)
{
    saveCurrentCode();

    QSettings settings("CppForge", "TaskWindow");
    settings.beginGroup("savedSolutions");
    for (auto it = savedSolutions_.begin(); it != savedSolutions_.end(); ++it)
    {
        settings.setValue(it.key(), it.value());
    }
    settings.endGroup();

    emit windowClosed();
    QWidget::closeEvent(event);
}

void TaskWindow::fadeIn()
{
    if (transitionAnimation_ && transitionAnimation_->state() == QPropertyAnimation::Running)
    {
        transitionAnimation_->stop();
    }

    setWindowOpacity(0.0);
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
    {
        transitionAnimation_->stop();
    }

    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(200);
    transitionAnimation_->setStartValue(1.0);
    transitionAnimation_->setEndValue(0.0);
    transitionAnimation_->setEasingCurve(QEasingCurve::InOutCubic);

    connect(transitionAnimation_.get(), &QPropertyAnimation::finished, this, [this]() { hide(); });

    transitionAnimation_->start();
}

void TaskWindow::setupUI()
{
    setWindowTitle("cppforge - Обучение");
    setMinimumSize(1200, 800);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("TaskWindow");

    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setTitle("cppforge - Обучение");

    auto mainContainer = new QWidget(this);
    mainContainer->setObjectName("mainContainer");

    auto mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    tabWidget_ = new QTabWidget();
    tabWidget_->setObjectName("tabWidget");

    theoryTab_ = new QWidget();
    auto theoryLayout = new QVBoxLayout(theoryTab_);
    theoryLayout->setContentsMargins(20, 20, 20, 20);
    theoryLayout->setSpacing(15);

    auto theoryTopPanel = new QHBoxLayout();

    auto theoryLabel = new QLabel("Раздел:");
    theoryLabel->setObjectName("sectionLabel");

    theorySelector_ = new QComboBox();
    theorySelector_->setObjectName("theorySelector");
    theorySelector_->setMinimumWidth(300);

    theoryTopPanel->addWidget(theoryLabel);
    theoryTopPanel->addWidget(theorySelector_);
    theoryTopPanel->addStretch();

    theoryDisplay_ = new QTextBrowser();
    theoryDisplay_->setObjectName("theoryDisplay");
    theoryDisplay_->setOpenExternalLinks(true);

    auto theoryBottomPanel = new QHBoxLayout();

    prevTheoryButton_ = new QPushButton("← Предыдущая");
    prevTheoryButton_->setObjectName("navButton");
    prevTheoryButton_->setCursor(Qt::PointingHandCursor);

    theoryPageLabel_ = new QLabel("Страница 1/1");
    theoryPageLabel_->setObjectName("pageLabel");
    theoryPageLabel_->setAlignment(Qt::AlignCenter);

    nextTheoryButton_ = new QPushButton("Следующая →");
    nextTheoryButton_->setObjectName("navButton");
    nextTheoryButton_->setCursor(Qt::PointingHandCursor);

    theoryBottomPanel->addWidget(prevTheoryButton_);
    theoryBottomPanel->addStretch();
    theoryBottomPanel->addWidget(theoryPageLabel_);
    theoryBottomPanel->addStretch();
    theoryBottomPanel->addWidget(nextTheoryButton_);

    theoryLayout->addLayout(theoryTopPanel);
    theoryLayout->addWidget(theoryDisplay_, 1);
    theoryLayout->addLayout(theoryBottomPanel);

    practiceTab_ = new QWidget();
    auto practiceLayout = new QVBoxLayout(practiceTab_);
    practiceLayout->setContentsMargins(20, 20, 20, 20);

    auto practiceTopPanel = new QHBoxLayout();

    auto taskLabel = new QLabel("Задание:");
    taskLabel->setObjectName("sectionLabel");

    taskSelector_ = new QComboBox();
    taskSelector_->setObjectName("taskSelector");
    taskSelector_->setMinimumWidth(300);

    practiceTopPanel->addWidget(taskLabel);
    practiceTopPanel->addWidget(taskSelector_);
    practiceTopPanel->addStretch();

    practiceSplitter_ = new QSplitter(Qt::Horizontal);

    taskPanel_ = new QWidget();
    taskPanel_->setObjectName("taskPanel");
    auto taskPanelLayout = new QVBoxLayout(taskPanel_);
    taskPanelLayout->setContentsMargins(20, 20, 20, 20);
    taskPanelLayout->setSpacing(15);

    taskTitle_ = new QLabel();
    taskTitle_->setObjectName("taskTitle");
    taskTitle_->setWordWrap(true);

    taskDescription_ = new QLabel();
    taskDescription_->setObjectName("taskDescription");
    taskDescription_->setWordWrap(true);
    taskDescription_->setAlignment(Qt::AlignTop);

    taskPanelLayout->addWidget(taskTitle_);
    taskPanelLayout->addWidget(taskDescription_);
    taskPanelLayout->addStretch();

    auto compilerPanel = new QWidget();
    compilerPanel->setObjectName("compilerPanel");
    auto compilerLayout = new QVBoxLayout(compilerPanel);
    compilerLayout->setContentsMargins(20, 20, 20, 20);
    compilerLayout->setSpacing(15);

    auto codeLabel = new QLabel("Ваш код:");
    codeLabel->setObjectName("codeLabel");

    codeEditor_ = new QTextEdit();
    codeEditor_->setObjectName("codeEditor");
    codeEditor_->setFont(QFont("Consolas", 11));

    auto ioLayout = new QHBoxLayout();

    auto inputWidget = new QWidget();
    inputWidget->setObjectName("ioPanel");
    auto inputLayout = new QVBoxLayout(inputWidget);
    inputLayout->setContentsMargins(10, 10, 10, 10);

    inputLayout->addWidget(new QLabel("Входные данные:"));
    inputField_ = new QTextEdit();
    inputField_->setObjectName("ioField");
    inputField_->setMaximumHeight(100);
    inputField_->setFont(QFont("Consolas", 11));
    inputLayout->addWidget(inputField_);

    auto outputWidget = new QWidget();
    outputWidget->setObjectName("ioPanel");
    auto outputLayout = new QVBoxLayout(outputWidget);
    outputLayout->setContentsMargins(10, 10, 10, 10);

    outputLayout->addWidget(new QLabel("Результат:"));
    outputField_ = new QTextEdit();
    outputField_->setObjectName("ioField");
    outputField_->setMaximumHeight(100);
    outputField_->setFont(QFont("Consolas", 11));
    outputField_->setReadOnly(true);
    outputLayout->addWidget(outputField_);

    ioLayout->addWidget(inputWidget);
    ioLayout->addWidget(outputWidget);

    auto buttonLayout = new QHBoxLayout();

    runButton_ = new QPushButton("Запустить");
    runButton_->setObjectName("actionButton");
    runButton_->setCursor(Qt::PointingHandCursor);

    checkButton_ = new QPushButton("Проверить");
    checkButton_->setObjectName("actionButton");
    checkButton_->setCursor(Qt::PointingHandCursor);
    checkButton_->setProperty("primary", true);

    resultLabel_ = new QLabel();
    resultLabel_->setObjectName("resultLabel");

    buttonLayout->addWidget(runButton_);
    buttonLayout->addWidget(checkButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(resultLabel_);

    compilerLayout->addWidget(codeLabel);
    compilerLayout->addWidget(codeEditor_, 3);
    compilerLayout->addLayout(ioLayout);
    compilerLayout->addLayout(buttonLayout);

    practiceSplitter_->addWidget(taskPanel_);
    practiceSplitter_->addWidget(compilerPanel);
    practiceSplitter_->setSizes({400, 800});

    practiceLayout->addLayout(practiceTopPanel);
    practiceLayout->addWidget(practiceSplitter_, 1);

    tabWidget_->addTab(theoryTab_, "📚 Теория");
    tabWidget_->addTab(practiceTab_, "💻 Практика");

    mainLayout->addWidget(tabWidget_);

    auto containerLayout = new QVBoxLayout(this);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->addWidget(customTitleBar_.get());

    auto marginWidget = new QWidget();
    auto marginLayout = new QVBoxLayout(marginWidget);
    marginLayout->setContentsMargins(20, 10, 20, 20);
    marginLayout->addWidget(mainContainer);
    containerLayout->addWidget(marginWidget);

    setStyleSheet(R"(
        #TaskWindow {
            background-color: transparent;
        }
        
        #mainContainer {
            background-color: white;
            border-radius: 24px;
            border: 1px solid rgba(0, 0, 0, 0.1);
        }
        
        QTabWidget::pane {
            border: none;
            background: transparent;
            border-radius: 16px;
        }
        
        QTabBar::tab {
            background: #f0f0f0;
            border: none;
            border-radius: 10px 10px 0 0;
            padding: 12px 30px;
            margin-right: 5px;
            font-size: 14px;
            font-weight: 500;
            color: #666;
        }
        
        QTabBar::tab:selected {
            background: white;
            color: #62639b;
            border-bottom: 3px solid #62639b;
        }
        
        QTabBar::tab:hover {
            background: #e0e0e0;
        }
        
        #sectionLabel {
            font-size: 14px;
            font-weight: 500;
            color: #555;
        }
        
        #theorySelector, #taskSelector {
            padding: 8px 15px;
            border: 1px solid #e0e0e0;
            border-radius: 10px;
            background: white;
            font-size: 14px;
            min-width: 250px;
        }
        
        #theorySelector:hover, #taskSelector:hover {
            border-color: #62639b;
        }
        
        #theoryDisplay {
            border: 1px solid #e0e0e0;
            border-radius: 16px;
            padding: 30px;
            background-color: #f8f9fc;
            font-size: 16px;
            line-height: 1.8;
        }
        
        #navButton {
            background-color: #f0f0f0;
            border: none;
            border-radius: 10px;
            padding: 10px 25px;
            font-size: 14px;
            font-weight: 500;
            color: #555;
            min-width: 150px;
        }
        
        #navButton:hover {
            background-color: #e0e0e0;
        }
        
        #navButton:disabled {
            background-color: #f5f5f5;
            color: #ccc;
        }
        
        #pageLabel {
            font-size: 14px;
            color: #777;
            padding: 0 20px;
        }
        
        #taskPanel {
            background-color: #f8f9fc;
            border-radius: 16px;
        }
        
        #taskTitle {
            font-size: 20px;
            font-weight: bold;
            color: #333;
            padding-bottom: 10px;
            border-bottom: 2px solid #62639b;
        }
        
        #taskDescription {
            font-size: 16px;
            color: #555;
            line-height: 1.6;
            padding: 10px 0;
        }
        
        #compilerPanel {
            background-color: white;
            border-radius: 16px;
        }
        
        #codeLabel {
            font-size: 14px;
            font-weight: 500;
            color: #333;
        }
        
        #codeEditor {
            border: 1px solid #e0e0e0;
            border-radius: 12px;
            padding: 15px;
            background: #f8f9fc;
            font-family: 'Consolas', monospace;
        }
        
        #ioPanel {
            background: #f8f9fc;
            border: 1px solid #e0e0e0;
            border-radius: 12px;
        }
        
        #ioField {
            border: none;
            background: transparent;
            font-family: 'Consolas', monospace;
            font-size: 12px;
            margin-top: 5px;
        }
        
        #actionButton {
            background: #f0f0f0;
            border: none;
            border-radius: 10px;
            padding: 10px 25px;
            font-weight: 500;
            font-size: 14px;
            min-width: 120px;
        }
        
        #actionButton:hover {
            background: #e0e0e0;
        }
        
        #actionButton[primary="true"] {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #62639b, stop:1 #7B7CB5);
            color: white;
        }
        
        #actionButton[primary="true"]:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #7B7CB5, stop:1 #62639b);
        }
        
        #resultLabel {
            font-size: 14px;
            font-weight: 500;
            padding: 0 15px;
        }
        
        QSplitter::handle {
            background: #e0e0e0;
            width: 2px;
        }
    )");
}

void TaskWindow::setupConnections()
{
    connect(tabWidget_, &QTabWidget::currentChanged, this, &TaskWindow::onTabChanged);

    connect(theorySelector_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TaskWindow::onTheorySelected);
    connect(prevTheoryButton_, &QPushButton::clicked, this,
            [this]()
            {
                if (currentTheoryIndex_ > 0)
                {
                    currentTheoryIndex_--;
                    updateTheoryContent();
                }
            });
    connect(nextTheoryButton_, &QPushButton::clicked, this,
            [this]()
            {
                if (currentTheoryIndex_ < theoryIds_.size() - 1)
                {
                    currentTheoryIndex_++;
                    updateTheoryContent();
                }
            });

    connect(taskSelector_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TaskWindow::onTaskSelected);
    connect(runButton_, &QPushButton::clicked, this, &TaskWindow::onRunCode);
    connect(checkButton_, &QPushButton::clicked, this, &TaskWindow::onCheckSolution);
}

void TaskWindow::loadModule(int moduleId)
{
    qDebug() << "Loading module:" << moduleId;
    currentModuleId_ = moduleId;

    Module module = taskManager_->getModule(moduleId);
    if (!module.isValid())
    {
        qDebug() << "Module" << moduleId << "not found";
        return;
    }

    theoryIds_.clear();
    if (!module.authorText.isEmpty())
    {
        theoryIds_.append("author_intro");
    }
    for (const auto &theory : module.theory)
    {
        theoryIds_.append(theory.id);
    }

    theorySelector_->clear();
    if (!module.authorText.isEmpty())
    {
        theorySelector_->addItem("📖 От автора", "author_intro");
    }
    for (const auto &theory : module.theory)
    {
        theorySelector_->addItem(theory.title, theory.id);
    }

    currentTheoryIndex_ = 0;
    updateTheoryContent();

    taskSelector_->clear();
    for (const auto &task : module.tasks)
    {
        QString difficultyIcon = task.difficulty == "easy" ? "🟢" : (task.difficulty == "medium" ? "🟡" : "🔴");
        taskSelector_->addItem(QString("%1 %2").arg(difficultyIcon).arg(task.title), task.id);
    }

    if (!module.tasks.isEmpty())
    {
        loadTask(module.tasks.first().id);
    }
}

void TaskWindow::onTabChanged(int index)
{
    qDebug() << "Switched to tab:" << index;
}

void TaskWindow::loadTask(int taskId)
{
    qDebug() << "Loading task:" << taskId;
    currentTaskId_ = taskId;
    updateTaskContent();
}

void TaskWindow::updateTaskContent()
{
    Task task = taskManager_->getTask(currentModuleId_, currentTaskId_);
    if (!task.isValid())
        return;

    taskTitle_->setText(task.title);
    taskDescription_->setText(task.description);

    QString key = QString("%1_%2").arg(currentModuleId_).arg(currentTaskId_);
    if (savedSolutions_.contains(key))
    {
        codeEditor_->setText(savedSolutions_[key]);
    }
    else
    {
        codeEditor_->setText(task.initialCode);
    }

    inputField_->clear();
    outputField_->clear();
    resultLabel_->clear();
}

void TaskWindow::updateTheoryContent()
{
    if (currentTheoryIndex_ < 0 || currentTheoryIndex_ >= theoryIds_.size())
        return;

    Module module = taskManager_->getModule(currentModuleId_);
    QString theoryId = theoryIds_[currentTheoryIndex_];

    theorySelector_->setCurrentIndex(currentTheoryIndex_);

    prevTheoryButton_->setEnabled(currentTheoryIndex_ > 0);
    nextTheoryButton_->setEnabled(currentTheoryIndex_ < theoryIds_.size() - 1);
    theoryPageLabel_->setText(QString("Страница %1/%2").arg(currentTheoryIndex_ + 1).arg(theoryIds_.size()));

    if (theoryId == "author_intro" && !module.authorText.isEmpty())
    {
        theoryDisplay_->setPlainText(module.authorText);
    }
    else
    {
        for (const auto &theory : module.theory)
        {
            if (theory.id == theoryId)
            {
                if (theory.format == "markdown")
                {
                    theoryDisplay_->setMarkdown(theory.content);
                }
                else
                {
                    theoryDisplay_->setPlainText(theory.content);
                }
                break;
            }
        }
    }

    theoryDisplay_->verticalScrollBar()->setValue(0);
}

void TaskWindow::onTheorySelected(int index)
{
    qDebug() << "Theory selected:" << index;
    if (index >= 0 && index != currentTheoryIndex_)
    {
        currentTheoryIndex_ = index;
        updateTheoryContent();
    }
}

void TaskWindow::onTaskSelected(int index)
{
    qDebug() << "Task selected:" << index;
    if (index >= 0)
    {
        saveCurrentCode();
        int taskId = taskSelector_->currentData().toInt();
        loadTask(taskId);
    }
}

void TaskWindow::onRunCode()
{
    qDebug() << "Running code...";
    saveCurrentCode();
    QString code = codeEditor_->toPlainText();
    QString input = inputField_->toPlainText();
    runCode(code, input);
}

void TaskWindow::onCheckSolution()
{
    qDebug() << "Checking solution...";
    saveCurrentCode();
    QString code = codeEditor_->toPlainText();

    resultLabel_->setText("Проверка...");
    resultLabel_->setStyleSheet("color: #666;");

    Task task = taskManager_->getTask(currentModuleId_, currentTaskId_);

    if (!task.isValid() || task.testCases.isEmpty())
    {
        outputField_->setText("Нет тестов для этого задания");
        return;
    }

    cppforge::entities::CodingTask codingTask(task.id, currentModuleId_, task.title, task.description, task.initialCode,
                                              {}, 1000, 256);
    if (analyzer_)
    {
        auto analysisResult = analyzer_->analyze(codingTask, code);
        if (analysisResult.has_value())
        {
            resultLabel_->setText("❌ Код не прошёл проверку");
            resultLabel_->setStyleSheet("color: #dc3545; font-weight: bold;");
            outputField_->setText("Ошибка статического анализа:\n" + analysisResult.value());
            return;
        }
    }

    std::vector<cppforge::entities::TestCase> coreTests;
    for (int i = 0; i < task.testCases.size(); ++i)
    {
        coreTests.push_back(
            cppforge::entities::TestCase(i + 1, task.testCases[i].input, task.testCases[i].expectedOutput, true));
    }

    auto result = codeRunner_->runBlocking(code, coreTests);

    if (!result.isSuccess() && result.getErrors() == "Compilation failed")
    {
        outputField_->setText("❌ Ошибка компиляции");
        resultLabel_->setText("❌ Ошибка компиляции");
        resultLabel_->setStyleSheet("color: #dc3545; font-weight: bold;");
        return;
    }

    if (result.isSuccess())
    {
        resultLabel_->setText("✅ Все тесты пройдены!");
        resultLabel_->setStyleSheet("color: #28a745; font-weight: bold;");

        int progress = calculateModuleProgress(currentModuleId_);
        emit moduleProgressUpdated(currentModuleId_, progress);

        if (progress == 100)
        {
            unlockNextModule(currentModuleId_);
        }
    }
    else
    {
        resultLabel_->setText("❌ Некоторые тесты не пройдены");
        resultLabel_->setStyleSheet("color: #dc3545; font-weight: bold;");
    }

    QString info = "Успешно пройдено тестов: " + QString::number(result.getPassedTestsCount()) + " из " +
                   QString::number(coreTests.size()) + "\n\n";
    if (!result.getOutput().isEmpty())
        info += "Вывод:\n" + result.getOutput() + "\n";
    if (!result.getErrors().isEmpty())
        info += "Ошибки:\n" + result.getErrors();

    outputField_->setText(info);
}

void TaskWindow::runCode(const QString &code, const QString &input)
{
    outputField_->setText("Компиляция и выполнение...");

    std::vector<cppforge::entities::TestCase> testCases;
    testCases.push_back(cppforge::entities::TestCase(1, input, "", true));

    auto result = codeRunner_->runBlocking(code, testCases);

    if (!result.isSuccess() && result.getErrors() == "Compilation failed")
    {
        outputField_->setText("Ошибка компиляции.");
    }
    else
    {
        QString finalStr;
        if (!result.getOutput().isEmpty())
            finalStr += "Вывод:\n" + result.getOutput();
        if (!result.getErrors().isEmpty())
            finalStr += "\nОшибки:\n" + result.getErrors();
        outputField_->setText(finalStr.trimmed());
    }
}

void TaskWindow::saveCurrentCode()
{
    if (currentModuleId_ != -1 && currentTaskId_ != -1)
    {
        QString key = QString("%1_%2").arg(currentModuleId_).arg(currentTaskId_);
        savedSolutions_[key] = codeEditor_->toPlainText();
    }
}

void TaskWindow::loadSavedCode() {}

int TaskWindow::calculateModuleProgress(int moduleId)
{
    if (!taskManager_)
        return 0;

    Module module = taskManager_->getModule(moduleId);
    if (!module.isValid() || module.tasks.isEmpty())
        return 0;

    int completedTasks = 0;
    for (const Task &task : module.tasks)
    {
        QString key = QString("%1_%2").arg(moduleId).arg(task.id);
        if (savedSolutions_.contains(key))
        {
            completedTasks++;
        }
    }

    return (completedTasks * 100) / module.tasks.size();
}

void TaskWindow::unlockNextModule(int moduleId)
{
    if (!taskManager_)
        return;

    int nextModuleId = moduleId + 1;
    if (taskManager_->isModuleAvailable(nextModuleId))
    {
        qDebug() << "Unlocking next module:" << nextModuleId;
        emit moduleProgressUpdated(nextModuleId, 0);
    }
}