#include "../include/signupwindow.hpp"

#include <QIcon>
#include <QDebug>

SignUpWindow::SignUpWindow(QWidget *parent) : QWidget(parent, Qt::Window)
{
    setAttribute(Qt::WA_TranslucentBackground, false);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setupUI();
}

void SignUpWindow::onSignUpButtonClicked()
{
    qDebug() << "SignUp button clicked"; // future
}

void SignUpWindow::onCloseButtonClicked()
{
    close();
}

void SignUpWindow::setupUI()
{
    setupWindowProperties();

    setupMainLabel();
    setupUsernameLabel();
    setupEmailLabel();
    setupPasswordLabel();

    setupEditFields();

    setupSignUpButton();
    setupCloseButton();

    setupLayout();

    setupConnections();
}

void SignUpWindow::setupWindowProperties()
{
    setFixedSize(500, 350);
    setWindowIcon(QIcon("../icons/icon.ico"));
    setWindowTitle("Authentication");
    setStyleSheet("background-color: #2e2e2e; border-radius: 10px;");
    setCursor(Qt::ArrowCursor);
}

void SignUpWindow::setupLabelFont(QLabel *label_)
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

void SignUpWindow::setupEditFieldFont(QLineEdit *editField_)
{
    editFieldFont_ = std::make_unique<QFont>();
    editFieldFont_->setFamily("Cascadia");
    editFieldFont_->setPixelSize(24);
    editFieldFont_->setBold(true);
    editField_->setFont(*editFieldFont_);
    editField_->setStyleSheet("background-color: transparent; "
                              "color: white; "
                              "border: none; "
                              "padding: 10px;");
}

void SignUpWindow::setupButtonFont(QPushButton *button_)
{
    buttonFont_ = std::make_unique<QFont>();
    buttonFont_->setFamily("Cascadia");
    buttonFont_->setPixelSize(24);
    buttonFont_->setBold(true);
    button_->setFont(*buttonFont_);
}

void SignUpWindow::setupMainLabel()
{
    mainLabel_ = std::make_unique<QLabel>("Sign Up");
    setupLabelFont(mainLabel_.get());
}

void SignUpWindow::setupUsernameLabel()
{
    usernameLabel_ = std::make_unique<QLabel>("Enter your username");
    setupLabelFont(usernameLabel_.get());
}

void SignUpWindow::setupEmailLabel()
{
    emailLabel_ = std::make_unique<QLabel>("Enter your Email");
    setupLabelFont(emailLabel_.get());
}

void SignUpWindow::setupPasswordLabel()
{
    passwordLabel_ = std::make_unique<QLabel>("Enter your password");
    setupLabelFont(passwordLabel_.get());
}

void SignUpWindow::setupEditFields()
{
    usernameInput_ = std::make_unique<QLineEdit>();
    setupEditFieldFont(usernameInput_.get());
    emailInput_ = std::make_unique<QLineEdit>();
    setupEditFieldFont(emailInput_.get());
    passwordInput_ = std::make_unique<QLineEdit>();
    setupEditFieldFont(passwordInput_.get());
}

void SignUpWindow::setupSignUpButton()
{
    signUpButton_ = std::make_unique<QPushButton>("Save");
    setupButtonFont(signUpButton_.get());
}

void SignUpWindow::setupCloseButton()
{
    closeButton_ = std::make_unique<QPushButton>("x", this);
    closeButton_->setFixedSize(30, 30);
    closeButton_->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: none;"
        "   font-size: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #aa3333;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #aa3333;"
        "}"
        "QPushButton:focus {"
        "   background-color: transparent;"
        "   outline: none;"
        "}"
        "QPushButton:default {"
        "   background-color: transparent;"
        "}");
    closeButton_->setCursor(Qt::PointingHandCursor);
    closeButton_->move(460, 10);
}

void SignUpWindow::setupLayout()
{
    mainLayout_ = std::make_unique<QVBoxLayout>(this);

    mainLayout_->addWidget(closeButton_.get());

    mainLayout_->addWidget(mainLabel_.get());

    mainLayout_->addWidget(usernameLabel_.get());
    mainLayout_->addWidget(usernameInput_.get());

    mainLayout_->addWidget(emailLabel_.get());
    mainLayout_->addWidget(emailInput_.get());

    mainLayout_->addWidget(passwordLabel_.get());
    mainLayout_->addWidget(passwordInput_.get());

    mainLayout_->addWidget(signUpButton_.get());
}

void SignUpWindow::setupConnections()
{
    connect(closeButton_.get(), &QPushButton::clicked, this, &SignUpWindow::onCloseButtonClicked);
    connect(signUpButton_.get(), &QPushButton::clicked, this, &SignUpWindow::onSignUpButtonClicked);
}
