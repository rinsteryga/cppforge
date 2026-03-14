#pragma once

#include <QPropertyAnimation>
#include <QWidget>
#include <memory>
#include "CustomTitleBar.hpp"

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
    // ОБЯЗАТЕЛЬНО для Frameless окон со стилями
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void centerWindow();

    std::unique_ptr<CustomTitleBar> customTitleBar_;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;
};