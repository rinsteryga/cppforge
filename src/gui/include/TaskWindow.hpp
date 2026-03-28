#pragma once

#include "../../core/include/entities/CodingTask.hpp"
#include "../../core/include/services/CodeRunner.hpp"
#include "../../core/include/services/StaticAnalyzer.hpp"
#include "CustomTitleBar.hpp"

#include <QDebug>
#include <QFutureWatcher>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QStackedWidget>

#include <memory>

// Forward declarations для ускорения компиляции
class QFrame;
class QListWidget;

class TaskWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = nullptr);
    ~TaskWindow();

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
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void setupStyles();
    void centerWindow();

    // Логика выпадающего меню
    void toggleNavMenu(bool open);

    void onRunClicked();
    void onSubmitClicked();

    void onNextTask();
    void onPrevTask();

    // Основные компоненты UI
    std::unique_ptr<CustomTitleBar> customTitleBar_;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;
    
    // Сайдбар и стек контента (Теория/Практика)
    QWidget* leftSidebar_{nullptr};       // Тот самый "пропавший" сайдбар
    QStackedWidget* contentStack_{nullptr}; // Контейнер для теории и практики

    // Анимация и виджеты выпадающего меню
    std::unique_ptr<QPropertyAnimation> navAnimation_;
    QFrame *navPanel_{nullptr};
    QListWidget *lessonList_{nullptr};

    // Элементы редактора и вывода
    QTextEdit *codeEditor_{nullptr};
    QTextEdit *testOutput_{nullptr};
    
    QTextEdit *practiceEdit_{nullptr};
    QTextEdit *theoryEdit_{nullptr};

    // Кнопки управления
    QPushButton *btnNext_{nullptr};
    QPushButton *btnPrev_{nullptr};
    QPushButton *btnBack_{nullptr};

    // Сервисы и данные
    std::unique_ptr<cppforge::services::CodeRunner> runner_;
    std::unique_ptr<cppforge::services::StaticAnalyzer> analyzer_;
    cppforge::entities::CodingTask currentTask_;

    int currentModuleId_{-1};
};