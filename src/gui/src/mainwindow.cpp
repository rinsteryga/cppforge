#include "../include/mainwindow.hpp"

#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), authWindow_(nullptr)
{
    setupUI();
}

void MainWindow::setupUI()
{
    centralWidget_ = std::make_unique<QWidget>(this);
    setCentralWidget(centralWidget_.get());

    setupMainLabel();
    setupMainButton();
    setupLayout();
    setupWindowProperties();
    setupConnections();
}

void MainWindow::setupLabelFont(QLabel *label_)
{
    labelFont_ = std::make_unique<QFont>();
    labelFont_->setFamily("Cascadia");
    labelFont_->setPixelSize(28);
    labelFont_->setBold(true);
    labelFont_->setItalic(true);
    label_->setFont(*labelFont_);
    label_->setWordWrap(true);
    label_->setStyleSheet("color: white;");
}

void MainWindow::setupWindowProperties()
{
    setFixedSize(900, 700);
    setWindowIcon(QIcon("../icon.ico"));
    setWindowTitle("diploma project");
    setStyleSheet("background-color: #1d201a28");
    setCursor(Qt::ArrowCursor);
}

void MainWindow::setupMainLabel()
{
    mainLabel_ = std::make_unique<QLabel>("my code learning application");
    setupLabelFont(mainLabel_.get());
    mainLabel_->setAlignment(Qt::AlignCenter);
}

void MainWindow::setupMainButtonFont()
{
    mainButtonFont_ = std::make_unique<QFont>();
    mainButtonFont_->setFamily("Cascadia");
    mainButtonFont_->setPixelSize(30);
    mainButtonFont_->setBold(true);
    mainButton_->setFont(*mainButtonFont_);
}

void MainWindow::setupMainButton()
{
    mainButton_ = std::make_unique<QPushButton>("Sign In");
    setupMainButtonFont();
    mainButton_->setCursor(Qt::PointingHandCursor);
    mainButton_->setFont(*mainButtonFont_);
    mainButton_->setStyleSheet("background-color: #b0b96aff; "
                               "color: white; "
                               "border: none; "
                               "padding: 10px;");
}

void MainWindow::setupConnections()
{
    connect(mainButton_.get(), &QPushButton::clicked, this, &MainWindow::onButtonClicked);
}

void MainWindow::setupLayout()
{
    mainLayout_ = std::make_unique<QVBoxLayout>(centralWidget_.get());
    mainLayout_->addWidget(mainLabel_.get());
    mainLayout_->addWidget(mainButton_.get());
}

void MainWindow::openAuthWindow()
{
    if (!authWindow_)
    {
        qDebug() << "Creating AuthWindow";
        authWindow_ = std::make_unique<AuthWindow>(this);
        authWindow_->setWindowModality(Qt::ApplicationModal);
    }
    qDebug() << "Showing AuthWindow";
    authWindow_->show();
    authWindow_->raise();
    authWindow_->activateWindow();
}

void MainWindow::onButtonClicked()
{
    qDebug() << "Button is clicked";
    openAuthWindow();
}
