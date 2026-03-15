#pragma once

#include "CustomTitleBar.hpp"

#include <QPropertyAnimation>
#include <QWidget>

#include <memory>

class TaskWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = nullptr);
    ~TaskWindow();

    void loadModule(int moduleId) {}
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

    std::unique_ptr<CustomTitleBar> customTitleBar_;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;
};