#pragma once

#include "AuthWindow.hpp"

#include <QtSql/QSqlDatabase>

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
    explicit MainWindow(QSqlDatabase& database, QWidget *parent = nullptr);
    explicit MainWindow(QWidget *parent = nullptr);
    
    ~MainWindow();

private:
    void onMainButtonClicked();
    void onCloseButtonClicked();
    void onMinimizeButtonClicked();

    void setupUI();

    void setupWindowProperties();
    void setupFaviconLabel();
    void setupWindowTitleLabel();

    void setupLabelFont(QLabel *label_);
    void setupMainButtonFont();
    
    void setupMainLabel();

    void setupMainButton();

    void setupCloseButton();
    void setupMinimizeButton();

    void setupTitleBar();

    void setupLayout();

    void openAuthWindow();

    void setupConnections();

    QSqlDatabase* database_ = nullptr;

    std::unique_ptr<QWidget> centralWidget_;

    std::unique_ptr<QLabel> mainLabel_;
    std::unique_ptr<QLabel> faviconLabel_;
    std::unique_ptr<QLabel> windowTitleLabel_;

    std::unique_ptr<QFont> labelFont_;
    std::unique_ptr<QFont> mainButtonFont_;

    std::unique_ptr<QWidget> titleBar_;

    std::unique_ptr<QPushButton> mainButton_;
    std::unique_ptr<QPushButton> closeButton_;
    std::unique_ptr<QPushButton> minimizeButton_;

    std::unique_ptr<QVBoxLayout> mainLayout_;

    std::unique_ptr<AuthWindow> authWindow_;
};