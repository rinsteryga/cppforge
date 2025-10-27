#include "../include/authwindow.hpp"

#include <QIcon>
#include <QDebug>

AuthWindow::AuthWindow(QWidget *parent) : QWidget(parent, Qt::Window)
{
    setAttribute(Qt::WA_TranslucentBackground, false);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setupUI();
}

void AuthWindow::onSignInButtonClicked()
{
    qDebug() << "LogIn button clicked"; // future
}

void AuthWindow::onCloseButtonClicked()
{
    close();
}

void AuthWindow::onSignUpButtonClicked()
{
    qDebug() << "SignUp button clicked";
    openSignUpWindow();
}

void AuthWindow::setupUI()
{
    setupWindowProperties();
    setupMainLabel();
    setupUsernameLabel();
    setupPasswordLabel();
    setupLogInButton();
    setupEditFields();
    setupCloseButton();
    setupSignUpLabel();
    setupSignUpButton();
    setupLayout();
    setupConnections();
}

void AuthWindow::setupWindowProperties()
{
    setFixedSize(650, 400);
    setWindowIcon(QIcon("../icons/icon.ico"));
    setWindowTitle("Authentication");
    setStyleSheet("background-color: #2e2e2e; border-radius: 10px;");
    setCursor(Qt::ArrowCursor);
}

void AuthWindow::setupLabelFont(QLabel *label_)
{
    labelFont_ = std::make_unique<QFont>();
    labelFont_->setFamily("Cascadia");
    labelFont_->setPixelSize(28);
    labelFont_->setBold(true);
    labelFont_->setItalic(true);
    label_->setStyleSheet("color: white;");
    label_->setWordWrap(true);
    label_->setFont(*labelFont_);
}

void AuthWindow::setupMainLabel()
{
    mainLabel_ = std::make_unique<QLabel>("Please log in");
    setupLabelFont(mainLabel_.get());
    mainLabel_->setAlignment(Qt::AlignCenter);
}

void AuthWindow::setupUsernameLabel()
{
    usernameLabel_ = std::make_unique<QLabel>("Username:");
    setupLabelFont(usernameLabel_.get());
}

void AuthWindow::setupPasswordLabel()
{
    passwordLabel_ = std::make_unique<QLabel>("Password:");
    setupLabelFont(passwordLabel_.get());
}

void AuthWindow::setupButtonFont(QPushButton *button_)
{
    auto buttonFont = std::make_unique<QFont>();
    buttonFont->setFamily("Cascadia");
    buttonFont->setPixelSize(30);
    buttonFont->setBold(true);
    button_->setCursor(Qt::PointingHandCursor);
    button_->setStyleSheet("background-color: #3db047ff; "
                           "color: white; "
                           "border: none; "
                           "padding: 10px;");
    button_->setFont(*buttonFont);
}

void AuthWindow::setupLogInButton()
{
    logInButton_ = std::make_unique<QPushButton>("Log In");
    setupButtonFont(logInButton_.get());
}

void AuthWindow::setupEditFieldFont(QLineEdit *editField_)
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

void AuthWindow::setupEditFields()
{
    usernameInput_ = std::make_unique<QLineEdit>();
    setupEditFieldFont(usernameInput_.get());
    passwordInput_ = std::make_unique<QLineEdit>();
    setupEditFieldFont(passwordInput_.get());
}

void AuthWindow::setupLayout()
{
    mainLayout_ = std::make_unique<QVBoxLayout>(this);
    mainLayout_->addWidget(closeButton_.get());
    mainLayout_->addWidget(mainLabel_.get());
    mainLayout_->addWidget(usernameLabel_.get());
    mainLayout_->addWidget(usernameInput_.get());
    mainLayout_->addWidget(passwordLabel_.get());
    mainLayout_->addWidget(passwordInput_.get());
    mainLayout_->addWidget(logInButton_.get());
    mainLayout_->addWidget(signUpLabel_.get());
    mainLayout_->addWidget(signUpButton_.get());
}

void AuthWindow::setupCloseButton()
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

void AuthWindow::setupConnections()
{
    connect(logInButton_.get(), &QPushButton::clicked, this, &AuthWindow::onSignInButtonClicked);
    connect(closeButton_.get(), &QPushButton::clicked, this, &AuthWindow::onCloseButtonClicked);
    connect(signUpButton_.get(), &QPushButton::clicked, this, &AuthWindow::onSignUpButtonClicked);
}

void AuthWindow::openSignUpWindow()
{
    if (!signUpWindow_)
    {
        qDebug() << "Creating SignUp Window";
        signUpWindow_ = std::make_unique<SignUpWindow>(this);
        signUpWindow_->setWindowModality(Qt::ApplicationModal);
    }
    qDebug() << "Showing SignUp Window";
    signUpWindow_->show();
    signUpWindow_->raise();
    signUpWindow_->activateWindow();
}

void AuthWindow::setupSignUpLabel()
{
    signUpLabel_ = std::make_unique<QLabel>("Don`t have an account?");
    setupLabelFont(signUpLabel_.get());
    signUpLabel_->setAlignment(Qt::AlignCenter);
}

void AuthWindow::setupSignUpButton()
{
    signUpButton_ = std::make_unique<QPushButton>("Sign Up", this);
    signUpButton_->setFixedSize(40, 40);
    signUpButton_->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: none;"
        "   font-size: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: transparent;"
        "}"
        "QPushButton:pressed {"
        "   background-color: transparent;"
        "}"
        "QPushButton:focus {"
        "   background-color: transparent;"
        "   outline: none;"
        "}"
        "QPushButton:default {"
        "   background-color: transparent;"
        "}");
    signUpButton_->setCursor(Qt::PointingHandCursor);
    signUpButton_->move(100, 30);
}
