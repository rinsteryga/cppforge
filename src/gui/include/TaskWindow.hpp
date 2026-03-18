#pragma once

#include "CustomTitleBar.hpp"

// Исправленные пути инклудов
#include "../../core/include/entities/CodingTask.hpp"
#include "../../core/include/services/CodeRunner.hpp"
#include "../../core/include/services/StaticAnalyzer.hpp"

#include <QPropertyAnimation>
#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QFutureWatcher>
#include <QDebug>
#include <memory>

class TaskWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = nullptr);
    ~TaskWindow();

    // Этот метод вызывается из MainWindow.cpp, его нужно обязательно объявить
    void loadModule(int moduleId); 

    void setTask(const cppforge::entities::CodingTask &task);
    void fadeIn();
    void fadeOut();

signals:
    void moduleProgressUpdated(int moduleId, int progress);
    void windowClosed();

protected:
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void setupStyles();
    void centerWindow();
    
    void onRunClicked();
    void onSubmitClicked();

    std::unique_ptr<CustomTitleBar> customTitleBar_;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;

    QTextEdit* codeEditor_{nullptr};
    QTextEdit* testOutput_{nullptr};
    QLabel* practiceContent_{nullptr};
    QLabel* theoryContent_{nullptr};

    std::unique_ptr<cppforge::services::CodeRunner> runner_;
    std::unique_ptr<cppforge::services::StaticAnalyzer> analyzer_;
    cppforge::entities::CodingTask currentTask_;
};