#include "SignUpWindow.hpp"

#include "CustomTitleBar.hpp"
#include "WindowStateManager.hpp"

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
#include <QStyleOption>
#include <QTimer>

SignUpWindow::SignUpWindow(std::shared_ptr<cppforge::services::AuthManager> authManager, QWidget *parent)
    : QWidget(parent, Qt::Window), passwordVisible_(false), authManager_(authManager)
{
    setupUI();

    WindowStateManager::instance().applyGeometry(this, QSize(1280, 900));
}

SignUpWindow::~SignUpWindow() = default;

void SignUpWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SignUpWindow::centerWindow()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QRect availableGeometry = screen->availableGeometry();
        int x = availableGeometry.x() + (availableGeometry.width() - width()) / 2;
        int y = availableGeometry.y() + (availableGeometry.height() - height()) / 2;
        move(x, y);
    }
}

void SignUpWindow::fadeIn()
{
    if (transitionAnimation_ && transitionAnimation_->state() == QAbstractAnimation::Running)
        transitionAnimation_->stop();

    setWindowOpacity(0.0);
    transitionAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    transitionAnimation_->setDuration(300);
    transitionAnimation_->setStartValue(0.0);
    transitionAnimation_->setEndValue(1.0);
    transitionAnimation_->setEasingCurve(QEasingCurve::InOutCubic);
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
    setMinimumSize(900, 600);
    resize(1280, 900);
    setWindowTitle("Sign Up - cppforge");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setObjectName("SignUpWindow");
    setupStyles();
}

void SignUpWindow::setupStyles()
{
    setStyleSheet(
        "#SignUpWindow { background-color: palette(window); border: 1px solid palette(mid); border-radius: 20px; }");
}

void SignUpWindow::setupTitleBar()
{
    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setTitle("Sign Up - cppforge");
    customTitleBar_->setIcon(windowIcon());
}

void SignUpWindow::setupLogo()
{
    if (!iconLabel_)
    {
        iconLabel_ = std::make_unique<QLabel>(this);
        iconLabel_->setFixedSize(200, 200);
    }

    QString logoPath = ":/icons/main_logo.ico";
    if (themeService_ && themeService_->getCurrentTheme() == cppforge::services::Theme::Dark)
    {
        logoPath = ":/icons/main_logo_dark.ico";
    }

    QPixmap pixmap(logoPath);
    if (pixmap.isNull())
    {
        showFallbackLogo();
    }
    else
    {
        iconLabel_->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    iconLabel_->setAlignment(Qt::AlignCenter);
    if (customTitleBar_)
    {
        customTitleBar_->setIcon(QIcon(logoPath));
    }
}

void SignUpWindow::showFallbackLogo()
{
    iconLabel_->setFixedSize(200, 200);
    iconLabel_->setStyleSheet("QLabel { background-color: #4285f4; border-radius: 20px; border: 4px solid #356ac3; "
                              "color: white; font-size: 48px; font-weight: bold; }");
    iconLabel_->setText("C++");
    iconLabel_->setAlignment(Qt::AlignCenter);
}

void SignUpWindow::setupTitle()
{
    titleLabel_ = std::make_unique<QLabel>("Create Your Account");
    QFont titleFont("Roboto", 32, QFont::Bold);
    titleLabel_->setFont(titleFont);
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setStyleSheet("color: palette(text); padding: 10px;");
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
        "QLineEdit { background-color: palette(alternate-base); color: palette(text); padding: 18px 20px; border: 2px "
        "solid palette(mid); border-radius: 10px; "
        "font-size: 18px; } QLineEdit:focus { border: 3px solid palette(link); outline: none; } "
        "QLineEdit::placeholder { font-size: 18px; color: palette(window-text); }");

    emailInput_ = std::make_unique<QLineEdit>();
    emailInput_->setPlaceholderText("Email address");
    emailInput_->setFixedHeight(65);
    emailInput_->setFixedWidth(500);
    emailInput_->setFont(inputFont);
    emailInput_->setStyleSheet("QLineEdit { background-color: palette(alternate-base); color: palette(text); padding: "
                               "18px 20px; border: 2px solid palette(mid); border-radius: 10px; "
                               "font-size: 18px; } QLineEdit:focus { border: 3px solid palette(link); outline: none; } "
                               "QLineEdit::placeholder { font-size: 18px; color: palette(window-text); }");

    passwordInput_ = std::make_unique<QLineEdit>();
    passwordInput_->setPlaceholderText("Password");
    passwordInput_->setEchoMode(QLineEdit::Password);
    passwordInput_->setFixedHeight(65);
    passwordInput_->setFixedWidth(500);
    passwordInput_->setFont(inputFont);
    passwordInput_->setStyleSheet(
        "QLineEdit { background-color: palette(alternate-base); color: palette(text); padding: 18px 50px 18px 20px; "
        "border: 2px solid palette(mid); border-radius: "
        "10px; font-size: 18px; } QLineEdit:focus { border: 3px solid palette(link); outline: "
        "none; } QLineEdit::placeholder { font-size: 18px; color: palette(window-text); }");

    passwordToggleButton_ = std::make_unique<QPushButton>();
    passwordToggleButton_->setFixedSize(32, 32);
    passwordToggleButton_->setCursor(Qt::PointingHandCursor);
    passwordToggleButton_->setStyleSheet(
        "QPushButton { background-color: transparent; border: none; padding: 4px; } QPushButton:hover { "
        "background-color: rgba(0, 0, 0, 0.05); border-radius: 4px; }");

    QPixmap openEyePixmap(":/images/eye_open.png");
    if (!openEyePixmap.isNull())
    {
        passwordToggleButton_->setIcon(
            QIcon(openEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        passwordToggleButton_->setIconSize(QSize(20, 20));
    }
}

void SignUpWindow::togglePasswordVisibility()
{
    passwordVisible_ = !passwordVisible_;
    passwordInput_->setEchoMode(passwordVisible_ ? QLineEdit::Normal : QLineEdit::Password);
    QString resourcePath = passwordVisible_ ? ":/images/eye_slash.png" : ":/images/eye_open.png";
    QPixmap eyePixmap(resourcePath);
    if (!eyePixmap.isNull())
    {
        passwordToggleButton_->setIcon(QIcon(eyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
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
    signUpButton_->setStyleSheet("QPushButton { background-color: palette(button); color: palette(button-text); "
                                 "border-radius: 12px; font-size: 24px; font-weight: "
                                 "bold; } QPushButton:hover { background-color: palette(highlight); } "
                                 "QPushButton:pressed { background-color: palette(highlight); }");
}

void SignUpWindow::setupBackToLoginLink()
{
    backToLoginButton_ = std::make_unique<QPushButton>("Already have an account? Sign in");
    backToLoginButton_->setFlat(true);
    backToLoginButton_->setFont(QFont("Roboto", 16));
    backToLoginButton_->setCursor(Qt::PointingHandCursor);
    backToLoginButton_->setStyleSheet("QPushButton { color: palette(link); background-color: transparent; border: "
                                      "none; font-size: 18px; padding: 15px; } "
                                      "QPushButton:hover { text-decoration: underline; }");
}

void SignUpWindow::setupLayout()
{
    mainLayout_ = std::make_unique<QVBoxLayout>(this);
    mainLayout_->setSpacing(0);
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->addWidget(customTitleBar_.get());

    auto centerContainer = std::make_unique<QWidget>();
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
    passwordContainer->setFixedSize(500, 65);
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

void SignUpWindow::setupConnections()
{
    connect(signUpButton_.get(), &QPushButton::clicked, this, &SignUpWindow::onSignUpButtonClicked);
    connect(backToLoginButton_.get(), &QPushButton::clicked, this, &SignUpWindow::onBackToLoginClicked);
    connect(passwordToggleButton_.get(), &QPushButton::clicked, this, &SignUpWindow::togglePasswordVisibility);
    connect(usernameInput_.get(), &QLineEdit::returnPressed, this, &SignUpWindow::onSignUpButtonClicked);
    connect(emailInput_.get(), &QLineEdit::returnPressed, this, &SignUpWindow::onSignUpButtonClicked);
    connect(passwordInput_.get(), &QLineEdit::returnPressed, this, &SignUpWindow::onSignUpButtonClicked);
}

void SignUpWindow::onSignUpButtonClicked()
{
    if (usernameInput_->text().isEmpty() || emailInput_->text().isEmpty() || passwordInput_->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please fill all fields");
        return;
    }

    if (authManager_ && authManager_->registerUser(usernameInput_->text(), emailInput_->text(), passwordInput_->text()))
    {
        QMessageBox::information(this, "Success", "Account created successfully!");
        WindowStateManager::instance().captureState(this);
        this->hide();
        emit switchToLogin();
    }
    else
    {
        QMessageBox::warning(this, "Error", "Registration failed.");
    }
}

void SignUpWindow::onBackToLoginClicked()
{
    if (transitionAnimation_)
        transitionAnimation_->stop();
    WindowStateManager::instance().captureState(this);
    this->hide();
    emit switchToLogin();
}

void SignUpWindow::setThemeService(cppforge::services::ThemeService *service)
{
    themeService_ = service;
    if (themeService_)
    {
        connect(themeService_, &cppforge::services::ThemeService::themeChanged, this,
                [this](cppforge::services::Theme)
                {
                    setupLogo();
                    setupStyles();
                    update();
                });
        setupLogo();
        setupStyles();
    }
}
