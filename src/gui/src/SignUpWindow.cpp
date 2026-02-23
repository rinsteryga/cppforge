#include "CustomTitleBar.hpp"
#include "SignUpWindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QIcon>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
#include <QSizePolicy>
#include <QTimer>
#include <QStyleOption>

SignUpWindow::SignUpWindow(std::shared_ptr<cppforge::services::AuthManager> authManager, QWidget *parent) 
    : QWidget(parent, Qt::Window),
      passwordVisible_(false),
      authManager_(authManager)
{    
    setupUI();
    QTimer::singleShot(50, this, &SignUpWindow::centerWindow);
}

SignUpWindow::~SignUpWindow() = default;

void SignUpWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
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

void SignUpWindow::fadeIn()
{
    setWindowOpacity(0.0);
    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(100);
    transitionAnimation_->setStartValue(0.0);
    transitionAnimation_->setEndValue(1.0);
    transitionAnimation_->start();
}

void SignUpWindow::setupUI()
{
    setupWindowProperties();
    setupTitleBar();
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
    setWindowTitle("Sign Up - cppforge");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("SignUpWindow");
    setStyleSheet("#SignUpWindow { background-color: white; border: 1px solid #cccccc; }");
}

void SignUpWindow::setupTitleBar()
{
    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setTitle("Sign Up - cppforge");
    customTitleBar_->setIcon(windowIcon());
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
    } else {
        showFallbackLogo();
    }
}

void SignUpWindow::showFallbackLogo() 
{
    iconLabel_->setFixedSize(200, 200);
    iconLabel_->setStyleSheet("QLabel {"
                            "   background-color: #4285f4;"
                            "   border-radius: 20px;"
                            "   border: 4px solid #356ac3;"
                            "   color: white;"
                            "   font-size: 48px;"
                            "   font-weight: bold;"
                            "}");
    iconLabel_->setText("C++");
    iconLabel_->setAlignment(Qt::AlignCenter);
}

void SignUpWindow::setupTitle()
{
    titleLabel_ = std::make_unique<QLabel>("Create Your Account");
    QFont titleFont("Roboto", 32, QFont::Bold);
    titleLabel_->setFont(titleFont);
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setStyleSheet("color: #000000; padding: 10px;");
    titleLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void SignUpWindow::setupInputFields()
{
    usernameInput_ = std::make_unique<QLineEdit>();
    usernameInput_->setPlaceholderText("Username");
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

void SignUpWindow::togglePasswordVisibility() {
  passwordVisible_ = !passwordVisible_;

  passwordInput_->setEchoMode(passwordVisible_ ? QLineEdit::Normal
                                               : QLineEdit::Password);

  QString resourcePath =
      passwordVisible_ ? ":/images/eye_slash.png" : ":/images/eye_open.png";

  QPixmap eyePixmap(resourcePath);

  if (!eyePixmap.isNull()) {
    eyePixmap =
        eyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    passwordToggleButton_->setIcon(QIcon(eyePixmap));
    passwordToggleButton_->setIconSize(QSize(20, 20));
    qDebug() << "SignUpWindow:" << (passwordVisible_ ? "Slash" : "Open")
             << "eye icon loaded";
  } else {
    qDebug() << "SignUpWindow: Eye icon not found:" << resourcePath;
  }
}

void SignUpWindow::setupSignUpButton()
{
    signUpButton_ = std::make_unique<QPushButton>("Create Account");
    signUpButton_->setFixedHeight(85);
    signUpButton_->setFixedWidth(500);
    
    QFont buttonFont("Roboto", 22, QFont::Bold);
    signUpButton_->setFont(buttonFont);
    signUpButton_->setCursor(Qt::PointingHandCursor);
    signUpButton_->setStyleSheet(
        "QPushButton {"
        "   background-color: #62639b;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   padding: 25px 30px;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   margin: 0px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7677B3;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #4B4C76;"
        "}"
    );
}

void SignUpWindow::setupBackToLoginLink()
{
    backToLoginButton_ = std::make_unique<QPushButton>("Already have an account? Sign in");
    backToLoginButton_->setFlat(true);
    
    QFont linkFont("Roboto", 16);
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
    mainLayout_->setSpacing(0);
    mainLayout_->setContentsMargins(0, 0, 0, 0);

    mainLayout_->addWidget(customTitleBar_.get());
    
    auto centerContainer = std::make_unique<QWidget>();
    centerContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto centerLayout = std::make_unique<QVBoxLayout>();
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
    
    auto passwordContainer = std::make_unique<QWidget>();
    passwordContainer->setFixedWidth(500);
    passwordContainer->setFixedHeight(65);
    
    passwordContainer->setLayout(nullptr);
    
    passwordInput_->setParent(passwordContainer.get());
    passwordInput_->setGeometry(0, 0, 500, 65);
    
    passwordToggleButton_->setParent(passwordContainer.get());
    passwordToggleButton_->setGeometry(500 - 42, 16, 32, 32);
    
    centerLayout->addWidget(passwordContainer.release(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(40);
    
    centerLayout->addWidget(signUpButton_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(30);
    
    centerLayout->addWidget(backToLoginButton_.get(), 0, Qt::AlignCenter);
    
    centerContainer->setLayout(centerLayout.release());
    mainLayout_->addWidget(centerContainer.release());
}

void SignUpWindow::setupConnections() {
  connect(signUpButton_.get(), &QPushButton::clicked, this,
          &SignUpWindow::onSignUpButtonClicked);
  connect(backToLoginButton_.get(), &QPushButton::clicked, this,
          &SignUpWindow::onBackToLoginClicked);
  connect(passwordToggleButton_.get(), &QPushButton::clicked, this,
          &SignUpWindow::togglePasswordVisibility);
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
    
    if (authManager_ && authManager_->registerUser(username, email, password))
    {
        QMessageBox::information(this, "Success", "Account created successfully!");
        
        transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
        transitionAnimation_->setDuration(150);
        transitionAnimation_->setStartValue(1.0);
        transitionAnimation_->setEndValue(0.0);
        connect(transitionAnimation_.get(), &QPropertyAnimation::finished, [this]() {
            emit switchToLogin();
            close();
        });
        transitionAnimation_->start();
    }
    else
    {
        QMessageBox::warning(this, "Error", "Registration failed. User may already exist.");
    }
}

void SignUpWindow::onBackToLoginClicked() {
  qDebug() << "Back to login clicked";
  
  transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
  transitionAnimation_->setDuration(150);
  transitionAnimation_->setStartValue(1.0);
  transitionAnimation_->setEndValue(0.0);
  connect(transitionAnimation_.get(), &QPropertyAnimation::finished, [this]() {
      emit switchToLogin();
      close();
  });
  transitionAnimation_->start();
}
