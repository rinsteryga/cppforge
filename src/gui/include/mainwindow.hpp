#pragma once

#include "../include/authwindow.hpp"

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QFont>
#include <QVBoxLayout>
#include <QPushButton>

#include <memory>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void onButtonClicked();

    void setupUI();
    void setupWindowProperties();
    void setupLabelFont(QLabel *label_);
    void setupMainLabel();
    void setupMainButtonFont();
    void setupMainButton();
    void setupLayout();

    void openAuthWindow();
    void setupConnections();

    std::unique_ptr<QWidget> centralWidget_;

    std::unique_ptr<QLabel> mainLabel_;

    std::unique_ptr<QFont> labelFont_;
    std::unique_ptr<QFont> mainButtonFont_;

    std::unique_ptr<QPushButton> mainButton_;

    std::unique_ptr<QVBoxLayout> mainLayout_;

    std::unique_ptr<AuthWindow> authWindow_;
};
