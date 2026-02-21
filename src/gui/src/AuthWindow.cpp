#include "AuthWindow.hpp"

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
#include <QStyleOption>

AuthWindow::AuthWindow(QWidget *parent)
    : QWidget(parent), signUpWindow_(nullptr), passwordVisible_(false)
{
    setupUI();
    QTimer::singleShot(50, this, &AuthWindow::centerWindow);
}

AuthWindow::~AuthWindow() = default;

void AuthWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void AuthWindow::setupWindowProperties() {
    setFixedSize(1280, 900);
    setWindowTitle("CppForge Log in");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("AuthWindow");
    setStyleSheet("#AuthWindow { background-color: white; border: 1px solid #cccccc; }");
}

void AuthWindow::setupTitleBar()
{
    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setTitle("Log in - CppForge");
    customTitleBar_->setIcon(windowIcon());
}

void AuthWindow::setupUI()
{   
    setupWindowProperties();
    setupTitleBar();
    setupLogo();
    setupTitle();
    setupInputFields();
    setupLoginButton();
    setupCreateAccountLink();
    setupLayout();
    
    connect(loginButton_.get(), &QPushButton::clicked, this, &AuthWindow::onLoginClicked);
    connect(createAccountButton_.get(), &QPushButton::clicked, this, &AuthWindow::onCreateAccountClicked);
    connect(passwordToggleButton_.get(), &QPushButton::clicked, this, &AuthWindow::togglePasswordVisibility);
}

void AuthWindow::centerWindow()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect availableGeometry = screen->availableGeometry();
        int x = availableGeometry.x() + (availableGeometry.width() - width()) / 2;
        int y = availableGeometry.y() + (availableGeometry.height() - height()) / 2;
        move(x, y);
    }
}

void AuthWindow::setupLogo()
{
    iconLabel_ = std::make_unique<QLabel>();
    iconLabel_->setAlignment(Qt::AlignCenter);
    iconLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QPixmap logoPixmap(":/icons/main_logo.ico");
    
    if (!logoPixmap.isNull()) {
        logoPixmap = logoPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel_->setPixmap(logoPixmap);
        iconLabel_->setFixedSize(200, 200);
        qDebug() << "Logo loaded";
    } else {
        qDebug() << "Logo not found, using fallback";
        showFallbackLogo();
    }
}

void AuthWindow::showFallbackLogo()
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

void AuthWindow::setupTitle()
{
    titleLabel_ = std::make_unique<QLabel>("Log into CppForge");
    QFont titleFont("Roboto", 32, QFont::Bold);
    titleLabel_->setFont(titleFont);
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setStyleSheet("color: #000000; padding: 10px;");
    titleLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void AuthWindow::setupInputFields()
{
    usernameInput_ = std::make_unique<QLineEdit>();
    usernameInput_->setPlaceholderText("Username or email address");
    usernameInput_->setFixedHeight(65);
    usernameInput_->setFixedWidth(500);
    
    QFont inputFont("Roboto", 16);
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
        qDebug() << "Open eye icon loaded";
    } else {
        qDebug() << "Open eye icon not found";
    }
}

void AuthWindow::togglePasswordVisibility()
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
        qDebug() << (passwordVisible_ ? "Slash" : "Open") << "eye icon loaded";
    } else {
        qDebug() << "Eye icon not found:" << resourcePath;
    }
}

void AuthWindow::setupLoginButton()
{
    loginButton_ = std::make_unique<QPushButton>("Log in");
    loginButton_->setFixedHeight(85);
    loginButton_->setFixedWidth(500);
    
    QFont buttonFont("Roboto", 22, QFont::Bold);
    loginButton_->setFont(buttonFont);
    loginButton_->setCursor(Qt::PointingHandCursor);
    loginButton_->setStyleSheet(
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
        "   background-color: #A5AEC9;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #97A2BD;"
        "}"
    );
}

void AuthWindow::setupCreateAccountLink()
{
    createAccountButton_ = std::make_unique<QPushButton>("New to CppForge? Create an account");
    createAccountButton_->setFlat(true);
    
    QFont linkFont("Roboto", 16);
    createAccountButton_->setFont(linkFont);
    createAccountButton_->setCursor(Qt::PointingHandCursor);
    createAccountButton_->setStyleSheet(
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

void AuthWindow::setupLayout()
{
    mainLayout_ = std::make_unique<QVBoxLayout>(this);
    mainLayout_->setSpacing(0);
    mainLayout_->setContentsMargins(0, 0, 0, 0);

    mainLayout_->addWidget(customTitleBar_.get());
    
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
    
    QWidget *passwordContainer = new QWidget();
    passwordContainer->setFixedWidth(500);
    passwordContainer->setFixedHeight(65);
    
    passwordInput_->setParent(passwordContainer);
    passwordInput_->setGeometry(0, 0, 500, 65);
    
    passwordToggleButton_->setParent(passwordContainer);
    passwordToggleButton_->setGeometry(500 - 42, 16, 32, 32);
    
    centerLayout->addWidget(passwordContainer, 0, Qt::AlignCenter);
    centerLayout->addSpacing(40);
    
    centerLayout->addWidget(loginButton_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(30);
    
    centerLayout->addWidget(createAccountButton_.get(), 0, Qt::AlignCenter);
    
    mainLayout_->addWidget(centerContainer);
}

void AuthWindow::onLoginClicked()
{
    QString username = usernameInput_->text();
    QString password = passwordInput_->text();
    
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

void AuthWindow::openSignUpWindow()
{
    if (!signUpWindow_) {
        signUpWindow_ = std::make_unique<SignUpWindow>();
        
        connect(signUpWindow_.get(), &SignUpWindow::switchToLogin, [this]() {
            this->show();
            this->raise();
            this->activateWindow();
        });
        
        connect(signUpWindow_.get(), &QObject::destroyed, [this]() {
            signUpWindow_.release();
        });
    }

    this->hide();
    signUpWindow_->show();
    signUpWindow_->raise();
    signUpWindow_->activateWindow();
}

void AuthWindow::onCreateAccountClicked()
{
    qDebug() << "Create Account button clicked";
    openSignUpWindow();
}