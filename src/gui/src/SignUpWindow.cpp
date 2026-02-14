#include "SignUpWindow.hpp"

#include <QMessageBox>
#include <QDebug>
#include <QFont>
#include <QScreen>
#include <QGuiApplication>
#include <QSizePolicy>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QApplication>
#include <QFontMetrics>
#include <QIcon>
#include <QPainter>

SignUpWindow::SignUpWindow(QWidget *parent) 
    : QWidget(parent, Qt::Window),
      passwordVisible_(false)
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    
    setupUI();
    
    QTimer::singleShot(50, this, &SignUpWindow::centerWindow);
}

void SignUpWindow::centerWindow()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect availableGeometry = screen->availableGeometry();
        int x = availableGeometry.x() + (availableGeometry.width() - width()) / 2;
        int y = availableGeometry.y() + (availableGeometry.height() - height()) / 2;
        move(x, y);
        qDebug() << "SignUpWindow centered at:" << x << "," << y;
    }
}

void SignUpWindow::setupUI()
{
    setupWindowProperties();
    setupLogo();
    setupTitle();
    setupInputFields();
    setupSignUpButton();
    setupBackToLoginLink();
    setupLayout();
    setupConnections();
}

void SignUpWindow::setupWindowProperties()
{
    
    setFixedSize(1280, 900); 
    
    setStyleSheet("background-color: white;");
}

void SignUpWindow::setupLogo()
{
    iconLabel_ = std::make_unique<QLabel>();
    iconLabel_->setAlignment(Qt::AlignCenter);
    iconLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QPixmap logoPixmap(":/icons/main_logo.ico");

    if (!logoPixmap.isNull()) {
        logoPixmap = logoPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel_->setPixmap(logoPixmap);
        iconLabel_->setFixedSize(200, 200);
        qDebug() << "SignUpWindow: Logo loaded from relative path";
    } else {
        qDebug() << "SignUpWindow: Logo not found, using fallback";
        showFallbackLogo();
    }
}

void SignUpWindow::showFallbackLogo()
{
    iconLabel_->setFixedSize(200, 200);
    iconLabel_->setStyleSheet(
        "QLabel {"
        "   background-color: #4285f4;"
        "   border-radius: 20px;"
        "   border: 4px solid #356ac3;"
        "   color: white;"
        "   font-size: 48px;"
        "   font-weight: bold;"
        "}"
    );
    iconLabel_->setText("C++");
    iconLabel_->setAlignment(Qt::AlignCenter);
}

void SignUpWindow::setupTitle()
{
    titleLabel_ = std::make_unique<QLabel>("Create Your Account");
    QFont titleFont("Segoe UI", 32, QFont::Bold);
    titleLabel_->setFont(titleFont);
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setStyleSheet("color: #333333; padding: 10px;");
    titleLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void SignUpWindow::setupInputFields()
{

    usernameInput_ = std::make_unique<QLineEdit>();
    usernameInput_->setPlaceholderText("Username");
    usernameInput_->setFixedHeight(65);
    usernameInput_->setFixedWidth(500);
    
    QFont inputFont("Segoe UI", 16);
    usernameInput_->setFont(inputFont);
    usernameInput_->setStyleSheet(
        "QLineEdit {"
        "   padding: 18px 20px;"
        "   border: 2px solid #cccccc;"
        "   border-radius: 10px;"
        "   font-size: 18px;"
        "}"
        "QLineEdit:focus {"
        "   border: 3px solid #4285f4;"
        "   outline: none;"
        "}"
        "QLineEdit::placeholder {"
        "   font-size: 18px;"
        "   color: #888888;"
        "}"
    );
    
    
    emailInput_ = std::make_unique<QLineEdit>();
    emailInput_->setPlaceholderText("Email address");
    emailInput_->setFixedHeight(65);
    emailInput_->setFixedWidth(500);
    
    emailInput_->setFont(inputFont);
    emailInput_->setStyleSheet(
        "QLineEdit {"
        "   padding: 18px 20px;"
        "   border: 2px solid #cccccc;"
        "   border-radius: 10px;"
        "   font-size: 18px;"
        "}"
        "QLineEdit:focus {"
        "   border: 3px solid #4285f4;"
        "   outline: none;"
        "}"
        "QLineEdit::placeholder {"
        "   font-size: 18px;"
        "   color: #888888;"
        "}"
    );
    
    passwordInput_ = std::make_unique<QLineEdit>();
    passwordInput_->setPlaceholderText("Password");
    passwordInput_->setEchoMode(QLineEdit::Password);
    passwordInput_->setFixedHeight(65);
    passwordInput_->setFixedWidth(500);
    
    passwordInput_->setFont(inputFont);
    passwordInput_->setStyleSheet(
        "QLineEdit {"
        "   padding: 18px 50px 18px 20px;" 
        "   border: 2px solid #cccccc;"
        "   border-radius: 10px;"
        "   font-size: 18px;"
        "}"
        "QLineEdit:focus {"
        "   border: 3px solid #4285f4;"
        "   outline: none;"
        "}"
        "QLineEdit::placeholder {"
        "   font-size: 18px;"
        "   color: #888888;"
        "}"
    );
    
    passwordToggleButton_ = std::make_unique<QPushButton>();
    passwordToggleButton_->setFixedSize(32, 32);
    passwordToggleButton_->setCursor(Qt::PointingHandCursor);
    passwordToggleButton_->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   padding: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(0, 0, 0, 0.05);"
        "   border-radius: 4px;"
        "}"
    );
    
    QPixmap openEyePixmap(":/images/eye_open.png");
    if (!openEyePixmap.isNull()) {
        openEyePixmap = openEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        passwordToggleButton_->setIcon(QIcon(openEyePixmap));
        passwordToggleButton_->setIconSize(QSize(20, 20));
        qDebug() << "SignUpWindow: Open eye icon loaded";
    } else {
        qDebug() << "SignUpWindow: Open eye icon not found";
    }
}

void SignUpWindow::togglePasswordVisibility()
{
    passwordVisible_ = !passwordVisible_;
    
    
    passwordInput_->setEchoMode(passwordVisible_ ? QLineEdit::Normal : QLineEdit::Password);
    

    QString resourcePath = passwordVisible_ ? 
        ":/images/eye_slash.png" : 
        ":/images/eye_open.png";
    
    QPixmap eyePixmap(resourcePath);
    
    if (!eyePixmap.isNull()) {
        eyePixmap = eyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        passwordToggleButton_->setIcon(QIcon(eyePixmap));
        passwordToggleButton_->setIconSize(QSize(20, 20));
        qDebug() << "SignUpWindow:" << (passwordVisible_ ? "Slash" : "Open") << "eye icon loaded";
    } else {
        qDebug() << "SignUpWindow: Eye icon not found:" << resourcePath;
    }
}

void SignUpWindow::setupSignUpButton()
{
    signUpButton_ = std::make_unique<QPushButton>("Create Account");
    signUpButton_->setFixedHeight(85);
    signUpButton_->setFixedWidth(500);
    
    QFont buttonFont("Segoe UI", 22, QFont::Bold);
    signUpButton_->setFont(buttonFont);
    signUpButton_->setCursor(Qt::PointingHandCursor);
    signUpButton_->setStyleSheet(
        "QPushButton {"
        "   background-color: #B3BAD5;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 25px 30px;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   margin: 0px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #B3BAD5;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #B3BAD5;"
        "}"
    );
}

void SignUpWindow::setupBackToLoginLink()
{
    backToLoginButton_ = std::make_unique<QPushButton>("Already have an account? Sign in");
    backToLoginButton_->setFlat(true);
    
    QFont linkFont("Segoe UI", 16);
    backToLoginButton_->setFont(linkFont);
    backToLoginButton_->setCursor(Qt::PointingHandCursor);
    backToLoginButton_->setStyleSheet(
        "QPushButton {"
        "   color: #4285f4;"
        "   background-color: transparent;"
        "   border: none;"
        "   font-size: 18px;"
        "   padding: 15px;"
        "}"
        "QPushButton:hover {"
        "   color: #3367d6;"
        "   text-decoration: underline;"
        "}"
        "QPushButton:pressed {"
        "   color: #2a56c6;"
        "}"
    );
}

void SignUpWindow::setupLayout()
{
    mainLayout_ = std::make_unique<QVBoxLayout>(this);
    mainLayout_->setAlignment(Qt::AlignCenter);
    mainLayout_->setSpacing(0);
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    
    auto *centerContainer = new QWidget();
    centerContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto *centerLayout = new QVBoxLayout(centerContainer);
    centerLayout->setAlignment(Qt::AlignCenter);
    centerLayout->setSpacing(25);  
    
    centerLayout->addWidget(iconLabel_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(20);
    
    centerLayout->addWidget(titleLabel_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(40);
    
    centerLayout->addWidget(usernameInput_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(20);
    
    centerLayout->addWidget(emailInput_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(20);
    
    QWidget *passwordContainer = new QWidget();
    passwordContainer->setFixedWidth(500);
    passwordContainer->setFixedHeight(65);
    
    passwordContainer->setLayout(nullptr);
    
    passwordInput_->setParent(passwordContainer);
    passwordInput_->setGeometry(0, 0, 500, 65);
    
    passwordToggleButton_->setParent(passwordContainer);
    passwordToggleButton_->setGeometry(500 - 42, 16, 32, 32);
    
    centerLayout->addWidget(passwordContainer, 0, Qt::AlignCenter);
    centerLayout->addSpacing(40);
    
    centerLayout->addWidget(signUpButton_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(30);
    
    centerLayout->addWidget(backToLoginButton_.get(), 0, Qt::AlignCenter);
    
    mainLayout_->addWidget(centerContainer);
}

void SignUpWindow::setupConnections()
{
    connect(signUpButton_.get(), &QPushButton::clicked, 
            this, &SignUpWindow::onSignUpButtonClicked);
    connect(backToLoginButton_.get(), &QPushButton::clicked,
            this, &SignUpWindow::onBackToLoginClicked);
    connect(passwordToggleButton_.get(), &QPushButton::clicked,
            this, &SignUpWindow::togglePasswordVisibility);
}

void SignUpWindow::onSignUpButtonClicked()
{
    QString username = usernameInput_->text();
    QString email = emailInput_->text();
    QString password = passwordInput_->text();
    

    if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields");
        return;
    }
    
    if (password.length() < 6) {
        QMessageBox::warning(this, "Error", "Password must be at least 6 characters");
        return;
    }
    
    if (!email.contains("@") || !email.contains(".")) {
        QMessageBox::warning(this, "Error", "Please enter a valid email address");
        return;
    }
    
    qDebug() << "SignUp attempt - Username:" << username 
             << "Email:" << email 
             << "Password length:" << password.length();
    
    QMessageBox::information(this, "Success", "Account created successfully!");
    
    emit switchToLogin();
    close();
}

void SignUpWindow::onBackToLoginClicked()
{
    this->hide();  

    if (parentWidget()) {
        parentWidget()->show();
        parentWidget()->raise();
        parentWidget()->activateWindow();
    }
}