<<<<<<< Updated upstream
#pragma once

#include "CustomTitleBar.hpp"
#include <services/CodeRunner.hpp>
#include <services/StaticAnalyzer.hpp>

#include <QComboBox>
#include <QLabel>
#include <QMap>
#include <QProcess>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QWidget>

#include <memory>

class TaskManager;

class TaskWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = nullptr);
    ~TaskWindow();

    void loadModule(int moduleId);
    void fadeIn();
    void fadeOut();

signals:
    void moduleProgressUpdated(int moduleId, int progress);
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onTabChanged(int index);
    void onTaskSelected(int index);
    void onTheorySelected(int index);
    void onRunCode();
    void onCheckSolution();

private:
    void setupUI();
    void setupConnections();
    void loadTask(int taskId);
    void updateTaskContent();
    void updateTheoryContent();
    void runCode(const QString &code, const QString &input);
    void saveCurrentCode();
    void loadSavedCode();
    int calculateModuleProgress(int moduleId);
    void unlockNextModule(int moduleId);

    TaskManager *taskManager_;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;

    QTabWidget *tabWidget_;
    QWidget *theoryTab_;
    QWidget *practiceTab_;

    QComboBox *theorySelector_;
    QTextBrowser *theoryDisplay_;
    QPushButton *prevTheoryButton_;
    QPushButton *nextTheoryButton_;
    QLabel *theoryPageLabel_;
    QStringList theoryIds_;
    int currentTheoryIndex_;

    QSplitter *practiceSplitter_;
    QWidget *taskPanel_;
    QLabel *taskTitle_;
    QLabel *taskDescription_;
    QComboBox *taskSelector_;
    QTextEdit *codeEditor_;
    QTextEdit *inputField_;
    QTextEdit *outputField_;
    QPushButton *runButton_;
    QPushButton *checkButton_;
    QLabel *resultLabel_;

    std::unique_ptr<CustomTitleBar> customTitleBar_;
    std::unique_ptr<cppforge::services::CodeRunner> codeRunner_;
    std::unique_ptr<cppforge::services::StaticAnalyzer> analyzer_;
    QString currentCode_;
    int currentModuleId_;
    int currentTaskId_;

    QMap<QString, QString> savedSolutions_;
=======
#pragma once

#include <QWidget>
#include <memory>

class CustomTitleBar;
class QPaintEvent;
class QResizeEvent;

class TaskWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = nullptr);
    ~TaskWindow();

    void loadModule(int id);
    void fadeIn();

signals:
    void moduleProgressUpdated(int moduleId, int progress);
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    int currentModuleId_;
    
    // Используем обычный указатель для контента, чтобы не ебаться с владением в конструкторе
    CustomTitleBar* customTitleBar_ = nullptr;
    QWidget* contentArea = nullptr;
>>>>>>> Stashed changes
};