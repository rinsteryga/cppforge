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
      passwordVisible(false)
{
    // Устанавливаем флаги для правильного поведения окна
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    
    setupUI();
    
    // Центрируем окно через таймер
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
    
    resize(1160, 800); 
    
    setStyleSheet("background-color: white;");
}

void SignUpWindow::setupLogo()
{
    iconLabel = std::make_unique<QLabel>();
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QPixmap logoPixmap("../src/gui/resources/images/main_logo.png");
    
    if (!logoPixmap.isNull()) {
        logoPixmap = logoPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel->setPixmap(logoPixmap);
        iconLabel->setFixedSize(200, 200);
        qDebug() << "SignUpWindow: Logo loaded from relative path";
    } else {
        qDebug() << "SignUpWindow: Logo not found, using fallback";
        showFallbackLogo();
    }
}

void SignUpWindow::showFallbackLogo()
{
    iconLabel->setFixedSize(200, 200);
    iconLabel->setStyleSheet(
        "QLabel {"
        "   background-color: #4285f4;"
        "   border-radius: 20px;"
        "   border: 4px solid #356ac3;"
        "   color: white;"
        "   font-size: 48px;"
        "   font-weight: bold;"
        "}"
    );
    iconLabel->setText("C++");
    iconLabel->setAlignment(Qt::AlignCenter);
}

void SignUpWindow::setupTitle()
{
    titleLabel = std::make_unique<QLabel>("Create Your Account");
    QFont titleFont("Segoe UI", 32, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #333333; padding: 10px;");
    titleLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void SignUpWindow::setupInputFields()
{

    usernameInput = std::make_unique<QLineEdit>();
    usernameInput->setPlaceholderText("Username");
    usernameInput->setFixedHeight(65);
    usernameInput->setFixedWidth(500);
    
    QFont inputFont("Segoe UI", 16);
    usernameInput->setFont(inputFont);
    usernameInput->setStyleSheet(
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
    
    
    emailInput = std::make_unique<QLineEdit>();
    emailInput->setPlaceholderText("Email address");
    emailInput->setFixedHeight(65);
    emailInput->setFixedWidth(500);
    
    emailInput->setFont(inputFont);
    emailInput->setStyleSheet(
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
    
    passwordInput = std::make_unique<QLineEdit>();
    passwordInput->setPlaceholderText("Password");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setFixedHeight(65);
    passwordInput->setFixedWidth(500);
    
    passwordInput->setFont(inputFont);
    passwordInput->setStyleSheet(
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
    
    passwordToggleButton = std::make_unique<QPushButton>();
    passwordToggleButton->setFixedSize(32, 32);
    passwordToggleButton->setCursor(Qt::PointingHandCursor);
    passwordToggleButton->setStyleSheet(
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
    
    QPixmap openEyePixmap("../src/gui/resources/icons/eye_open.png");
    if (!openEyePixmap.isNull()) {
        openEyePixmap = openEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        passwordToggleButton->setIcon(QIcon(openEyePixmap));
        passwordToggleButton->setIconSize(QSize(20, 20));
        qDebug() << "SignUpWindow: Open eye icon loaded";
    } else {
        qDebug() << "SignUpWindow: Open eye icon not found";
    }
}

void SignUpWindow::togglePasswordVisibility()
{
    passwordVisible = !passwordVisible;
    
    
    passwordInput->setEchoMode(passwordVisible ? QLineEdit::Normal : QLineEdit::Password);
    

    QString resourcePath = passwordVisible ? 
        "../src/gui/resources/icons/eye_slash.png" : 
        "../src/gui/resources/icons/eye_open.png";
    
    QPixmap eyePixmap(resourcePath);
    
    if (!eyePixmap.isNull()) {
        eyePixmap = eyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        passwordToggleButton->setIcon(QIcon(eyePixmap));
        passwordToggleButton->setIconSize(QSize(20, 20));
        qDebug() << "SignUpWindow:" << (passwordVisible ? "Slash" : "Open") << "eye icon loaded";
    } else {
        qDebug() << "SignUpWindow: Eye icon not found:" << resourcePath;
    }
}

void SignUpWindow::setupSignUpButton()
{
    signUpButton = std::make_unique<QPushButton>("Create Account");
    signUpButton->setFixedHeight(85);
    signUpButton->setFixedWidth(500);
    
    QFont buttonFont("Segoe UI", 22, QFont::Bold);
    signUpButton->setFont(buttonFont);
    signUpButton->setCursor(Qt::PointingHandCursor);
    signUpButton->setStyleSheet(
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
    backToLoginButton = std::make_unique<QPushButton>("Already have an account? Sign in");
    backToLoginButton->setFlat(true);
    
    QFont linkFont("Segoe UI", 16);
    backToLoginButton->setFont(linkFont);
    backToLoginButton->setCursor(Qt::PointingHandCursor);
    backToLoginButton->setStyleSheet(
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
    mainLayout = std::make_unique<QVBoxLayout>(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(100, 50, 100, 50);
    
    auto *centerContainer = new QWidget();
    centerContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto *centerLayout = new QVBoxLayout(centerContainer);
    centerLayout->setAlignment(Qt::AlignCenter);
    centerLayout->setSpacing(25);  
    
    centerLayout->addWidget(iconLabel.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(20);
    
    centerLayout->addWidget(titleLabel.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(40);
    
    centerLayout->addWidget(usernameInput.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(20);
    
    centerLayout->addWidget(emailInput.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(20);
    
    QWidget *passwordContainer = new QWidget();
    passwordContainer->setFixedWidth(500);
    passwordContainer->setFixedHeight(65);
    
    passwordContainer->setLayout(nullptr);
    
    passwordInput->setParent(passwordContainer);
    passwordInput->setGeometry(0, 0, 500, 65);
    
    passwordToggleButton->setParent(passwordContainer);
    passwordToggleButton->setGeometry(500 - 42, 16, 32, 32);
    
    centerLayout->addWidget(passwordContainer, 0, Qt::AlignCenter);
    centerLayout->addSpacing(40);
    
    centerLayout->addWidget(signUpButton.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(30);
    
    centerLayout->addWidget(backToLoginButton.get(), 0, Qt::AlignCenter);
    
    mainLayout->addWidget(centerContainer);
}

void SignUpWindow::setupConnections()
{
    connect(signUpButton.get(), &QPushButton::clicked, 
            this, &SignUpWindow::onSignUpButtonClicked);
    connect(backToLoginButton.get(), &QPushButton::clicked,
            this, &SignUpWindow::onBackToLoginClicked);
    connect(passwordToggleButton.get(), &QPushButton::clicked,
            this, &SignUpWindow::togglePasswordVisibility);
}

void SignUpWindow::onSignUpButtonClicked()
{
    QString username = usernameInput->text();
    QString email = emailInput->text();
    QString password = passwordInput->text();
    

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
    qDebug() << "Back to login clicked";
    emit switchToLogin();
    close();
}