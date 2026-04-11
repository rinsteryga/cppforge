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
#include <QStackedWidget>
#include <QTextEdit>
#include <QWidget>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <memory>

class QFrame;

class TaskWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = nullptr);
    ~TaskWindow();

    void setUserId(int64_t id);

    void loadModule(int lessonId);

    void setTask(const cppforge::entities::CodingTask &task);

    void fadeIn();
    void fadeOut();

signals:
    void moduleProgressUpdated(int id, int progress);
    void windowClosed();

protected:
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onRunClicked();
    void onSubmitClicked();
    void onNextTask();
    void onPrevTask();

private:
    void setupUI();
    void setupStyles();
    void centerWindow();

    int getModuleProgress(int moduleId);

    std::unique_ptr<CustomTitleBar> customTitleBar_;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;

    QStackedWidget *contentStack_{nullptr};

    QTextEdit *codeEditor_{nullptr};
    QTextEdit *testOutput_{nullptr};
    QTextEdit *practiceEdit_{nullptr};
    QTextEdit *theoryEdit_{nullptr};

    QPushButton *btnNext_{nullptr};
    QPushButton *btnPrev_{nullptr};
    QPushButton *btnBack_{nullptr};
    QPushButton *btnSubmit_{nullptr};
    QPushButton *btnRun_{nullptr};

    std::unique_ptr<cppforge::services::CodeRunner> runner_;
    std::unique_ptr<cppforge::services::StaticAnalyzer> analyzer_;

    cppforge::entities::CodingTask currentTask_;

    int currentUserId_{1};
    int currentModuleId_{-1};
    int currentModuleParentId_{-1};
    bool hasCodingTask_{false};
};