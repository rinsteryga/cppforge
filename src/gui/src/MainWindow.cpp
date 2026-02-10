#include "MainWindow.hpp"

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), signUpWindow_(nullptr), passwordVisible(false)
{
    setupUI();
    
    QTimer::singleShot(50, this, &MainWindow::centerWindow);
}

void MainWindow::setupWindowProperties() {
    resize(1160, 800); 
    setWindowTitle("CppForge Log in");
    setWindowIcon(QIcon("../src/gui/resources/images/main_logo.png"));
    
    setStyleSheet("background-color: white;");
    
    centralWidget = std::make_unique<QWidget>(this);
    setCentralWidget(centralWidget.get());
}

void MainWindow::setupUI()
{   
    setupWindowProperties();
    setupLogo();
    setupTitle();
    setupInputFields();
    setupLoginButton();
    setupCreateAccountLink();
    setupLayout();
    
    connect(loginButton.get(), &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(createAccountButton.get(), &QPushButton::clicked, this, &MainWindow::onCreateAccountClicked);
    connect(passwordToggleButton.get(), &QPushButton::clicked, this, &MainWindow::togglePasswordVisibility);
}

void MainWindow::centerWindow()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect availableGeometry = screen->availableGeometry();
        int x = availableGeometry.x() + (availableGeometry.width() - width()) / 2;
        int y = availableGeometry.y() + (availableGeometry.height() - height()) / 2;
        move(x, y);
    }
}

void MainWindow::setupLogo()
{
    iconLabel = std::make_unique<QLabel>();
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QPixmap logoPixmap("../src/gui/resources/images/main_logo.png");
    
    if (!logoPixmap.isNull()) {
        logoPixmap = logoPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel->setPixmap(logoPixmap);
        iconLabel->setFixedSize(200, 200);
        qDebug() << "Logo loaded from relative path";
    } else {
        qDebug() << "Logo not found, using fallback";
        showFallbackLogo();
    }
}

void MainWindow::showFallbackLogo()
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

void MainWindow::setupTitle()
{
    titleLabel = std::make_unique<QLabel>("Log into CppForge");
    QFont titleFont("Segoe UI", 32, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #333333; padding: 10px;");
    titleLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void MainWindow::setupInputFields()
{
    usernameInput = std::make_unique<QLineEdit>();
    usernameInput->setPlaceholderText("Username or email address");
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
        qDebug() << "Open eye icon loaded from relative path";
    } else {
        qDebug() << "Open eye icon not found in relative path";
    }
}

void MainWindow::togglePasswordVisibility()
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
        qDebug() << (passwordVisible ? "Slash" : "Open") << "eye icon loaded from relative path";
    } else {
        qDebug() << "Eye icon not found:" << resourcePath;
    }
}

void MainWindow::setupLoginButton()
{
    loginButton = std::make_unique<QPushButton>("Log in");
    loginButton->setFixedHeight(85);
    loginButton->setFixedWidth(500);
    
    QFont buttonFont("Segoe UI", 22, QFont::Bold);
    loginButton->setFont(buttonFont);
    loginButton->setCursor(Qt::PointingHandCursor);
    loginButton->setStyleSheet(
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

void MainWindow::setupCreateAccountLink()
{
    createAccountButton = std::make_unique<QPushButton>("New to CppForge? Create an account");
    createAccountButton->setFlat(true);
    
    QFont linkFont("Segoe UI", 16);
    createAccountButton->setFont(linkFont);
    createAccountButton->setCursor(Qt::PointingHandCursor);
    createAccountButton->setStyleSheet(
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

void MainWindow::setupLayout()
{
    mainLayout = std::make_unique<QVBoxLayout>(centralWidget.get());
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(100, 50, 100, 50);
    
    auto *centerContainer = new QWidget();
    centerContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto *centerLayout = new QVBoxLayout(centerContainer);
    centerLayout->setAlignment(Qt::AlignCenter);
    centerLayout->setSpacing(35);
    
    centerLayout->addWidget(iconLabel.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(20);
    centerLayout->addWidget(titleLabel.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(40);
    centerLayout->addWidget(usernameInput.get(), 0, Qt::AlignCenter);
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
    centerLayout->addWidget(loginButton.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(createAccountButton.get(), 0, Qt::AlignCenter);
    
    mainLayout->addWidget(centerContainer);
}

void MainWindow::onLoginClicked()
{
    QString username = usernameInput->text();
    QString password = passwordInput->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter username and password");
        return;
    }
    
    QMessageBox::information(this, "Login", 
        QString("Login attempt with:\nUsername: %1\nPassword: %2")
            .arg(username)
            .arg(QString("*").repeated(password.length()))
    );
}

void MainWindow::openSignUpWindow() {
    qDebug() << "Opening SignUpWindow...";
    
    if (!signUpWindow_) {
        qDebug() << "Creating new SignUpWindow instance";
        
        // Создаем как независимое окно
        signUpWindow_ = std::make_unique<SignUpWindow>(nullptr);
        
        // Устанавливаем заголовок и иконку
        signUpWindow_->setWindowTitle("Create Account - CppForge");
        
        // Соединяем сигнал возврата
        connect(signUpWindow_.get(), &SignUpWindow::switchToLogin, [this]() {
            qDebug() << "Received switchToLogin signal";
            
            if (signUpWindow_) {
                signUpWindow_->close();
                signUpWindow_.reset();
            }
            
            // Активируем MainWindow
            this->setEnabled(true);
            this->show();
            this->raise();
            this->activateWindow();
        });
        
        qDebug() << "SignUpWindow created and signals connected";
    }

    // Делаем MainWindow неактивным
    this->setEnabled(false);
    
    // Показываем SignUpWindow
    signUpWindow_->show();
    signUpWindow_->raise();
    signUpWindow_->activateWindow();
    
    qDebug() << "SignUpWindow shown, MainWindow disabled";
}

void MainWindow::onCreateAccountClicked()
{
    qDebug() << "Create Account button clicked";
    openSignUpWindow();
}