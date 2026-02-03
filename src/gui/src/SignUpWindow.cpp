// SignUpWindow.cpp
#include "SignUpWindow.hpp"
#include <QIcon>
#include <QDebug>

// Конструкторы
SignUpWindow::SignUpWindow(QSqlDatabase& database, QWidget *parent)
    : QWidget(parent, Qt::Window), database_(&database)
{
    setAttribute(Qt::WA_TranslucentBackground, false);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setupUI();
}

SignUpWindow::SignUpWindow(QWidget *parent)
    : QWidget(parent, Qt::Window), database_(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground, false);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setupUI();
}

// Реализации методов...
void SignUpWindow::onSignUpButtonClicked()
{
    qDebug() << "SignUp button clicked";
    if (database_ && database_->isOpen())
    {
        qDebug() << "Database is available for registration";
    }
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
    setupConfirmPasswordLabel();
    setupSignUpButton();
    setupEditFields();
    setupCloseButton();
    setupLayout();
    setupConnections();
}

void SignUpWindow::setupWindowProperties()
{
    setFixedSize(650, 500);
    setWindowIcon(QIcon("../icons/icon.ico"));
    setWindowTitle("Sign Up");
    setStyleSheet("background-color: #2e2e2e; border-radius: 10px;");
    setCursor(Qt::ArrowCursor);
}

void SignUpWindow::setupLabelFont(QLabel *label_)
{
    labelFont_ = std::make_unique<QFont>();
    labelFont_->setFamily("Cascadia");
    labelFont_->setPixelSize(24);
    labelFont_->setBold(true);
    label_->setStyleSheet("color: white;");
    label_->setWordWrap(true);
    label_->setFont(*labelFont_);
}

void SignUpWindow::setupMainLabel()
{
    mainLabel_ = std::make_unique<QLabel>("Create Account");
    setupLabelFont(mainLabel_.get());
    mainLabel_->setAlignment(Qt::AlignCenter);
}

void SignUpWindow::setupUsernameLabel()
{
    usernameLabel_ = std::make_unique<QLabel>("Username:");
    setupLabelFont(usernameLabel_.get());
}

void SignUpWindow::setupEmailLabel()
{
    emailLabel_ = std::make_unique<QLabel>("Email:");
    setupLabelFont(emailLabel_.get());
}

void SignUpWindow::setupPasswordLabel()
{
    passwordLabel_ = std::make_unique<QLabel>("Password:");
    setupLabelFont(passwordLabel_.get());
}

void SignUpWindow::setupConfirmPasswordLabel()
{
    confirmPasswordLabel_ = std::make_unique<QLabel>("Confirm Password:");
    setupLabelFont(confirmPasswordLabel_.get());
}

void SignUpWindow::setupButtonFont(QPushButton *button_)
{
    auto buttonFont = std::make_unique<QFont>();
    buttonFont->setFamily("Cascadia");
    buttonFont->setPixelSize(28);
    buttonFont->setBold(true);
    button_->setCursor(Qt::PointingHandCursor);
    button_->setStyleSheet("background-color: #3db047ff; "
                           "color: white; "
                           "border: none; "
                           "padding: 10px;");
    button_->setFont(*buttonFont);
}

void SignUpWindow::setupSignUpButton()
{
    signUpButton_ = std::make_unique<QPushButton>("Sign Up");
    setupButtonFont(signUpButton_.get());
}

void SignUpWindow::setupEditFieldFont(QLineEdit *editField_)
{
    editFieldFont_ = std::make_unique<QFont>();
    editFieldFont_->setFamily("Cascadia");
    editFieldFont_->setPixelSize(20);
    editFieldFont_->setBold(true);
    editField_->setFont(*editFieldFont_);
    editField_->setStyleSheet("background-color: transparent; "
                              "color: white; "
                              "border: none; "
                              "padding: 10px;");
}

void SignUpWindow::setupEditFields()
{
    usernameInput_ = std::make_unique<QLineEdit>();
    setupEditFieldFont(usernameInput_.get());
    emailInput_ = std::make_unique<QLineEdit>();
    setupEditFieldFont(emailInput_.get());
    passwordInput_ = std::make_unique<QLineEdit>();
    passwordInput_->setEchoMode(QLineEdit::Password);
    setupEditFieldFont(passwordInput_.get());
    confirmPasswordInput_ = std::make_unique<QLineEdit>();
    confirmPasswordInput_->setEchoMode(QLineEdit::Password);
    setupEditFieldFont(confirmPasswordInput_.get());
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
    mainLayout_->addWidget(confirmPasswordLabel_.get());
    mainLayout_->addWidget(confirmPasswordInput_.get());
    mainLayout_->addWidget(signUpButton_.get());
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

void SignUpWindow::setupConnections()
{
    connect(signUpButton_.get(), &QPushButton::clicked, this, &SignUpWindow::onSignUpButtonClicked);
    connect(closeButton_.get(), &QPushButton::clicked, this, &SignUpWindow::onCloseButtonClicked);
}