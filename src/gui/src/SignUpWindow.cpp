#include "SignUpWindow.hpp"

#include "CustomTitleBar.hpp"
#include "WindowStateManager.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFont>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QIcon>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
#include <QScrollArea>
#include <QSettings>
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
    setMinimumSize(600, 500);
    setWindowTitle("Sign Up - cppforge");
    setWindowIcon(QIcon(":/icons/main_logo_pale.ico"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setObjectName("SignUpWindow");
    setupStyles();
}

void SignUpWindow::setupStyles()
{
    bool isDark = false;
    if (themeService_)
    {
        isDark = (themeService_->getCurrentTheme() == cppforge::services::Theme::Dark);
    }
    else
    {
        QSettings settings("CppForge", "StudyApp");
        isDark = (settings.value("app/theme", 0).toInt() == 1);
    }

    QString btnColor = isDark ? "#0e639c" : "#62639b";
    QString btnHover = isDark ? "#1177bb" : "#f3e8ff";
    QString hoverText = isDark ? "white" : "black";
    QString linkColor = isDark ? "#3794ff" : "#62639b";

    QString style = QString(R"(
        #SignUpWindow, #centerContainer { background-color: palette(window); border: none; }
        
        QPushButton#signUpButton { 
            background-color: %1; 
            color: white; 
            border-radius: 12px; 
            font-size: 24px; 
            font-weight: bold; 
            border: none; 
        }
        QPushButton#signUpButton:hover { background-color: %2; color: %3; }
        
        QPushButton#backToLoginButton { 
            color: %4; 
            background-color: transparent; 
            border: none; 
            border-bottom: 1px solid transparent; 
            font-size: 18px; 
            padding: 15px; 
        }
        QPushButton#backToLoginButton:hover { color: %2; border-bottom: 1px solid %2; }
        
        QLineEdit { 
            background-color: palette(alternate-base); 
            color: palette(text); 
            padding: 12px 20px; 
            border: 2px solid palette(mid); 
            border-radius: 10px; 
            font-size: 18px; 
        }
        QLineEdit:focus { border: 2px solid %1; }
    )")
                        .arg(btnColor)
                        .arg(btnHover)
                        .arg(hoverText)
                        .arg(linkColor);

    setStyleSheet(style);
    if (centerContainer_)
        centerContainer_->setStyleSheet(style);
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
        iconLabel_->setMinimumSize(120, 120);
        iconLabel_->setMaximumSize(200, 200);
        iconLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    QString logoPath = ":/icons/main_logo_pale.ico";
    if (themeService_ && themeService_->getCurrentTheme() == cppforge::services::Theme::Dark)
    {
        logoPath = ":/icons/main_logo_dark.ico";
    }

    QIcon logoIcon(logoPath);
    QPixmap pixmap = logoIcon.pixmap(200, 200);

    if (!pixmap.isNull())
    {
        iconLabel_->setPixmap(pixmap);
        iconLabel_->setText("");
        iconLabel_->setStyleSheet("background-color: transparent; border: none;");
    }
    else
    {
        iconLabel_->clear();
        iconLabel_->setStyleSheet("background-color: transparent; border: none;");
    }
    iconLabel_->setAlignment(Qt::AlignCenter);
    if (customTitleBar_)
    {
        customTitleBar_->setIcon(QIcon(logoPath));
    }
}

void SignUpWindow::showFallbackLogo()
{
    iconLabel_->setStyleSheet("background-color: transparent; border: none;");
    iconLabel_->setText("");
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
    usernameInput_->setMinimumSize(300, 50);
    usernameInput_->setMaximumSize(500, 65);

    QFont inputFont("Roboto", 16);
    usernameInput_->setFont(inputFont);
    usernameInput_->setStyleSheet(
        "QLineEdit { background-color: palette(alternate-base); color: palette(text); padding: 12px 20px; border: 2px "
        "solid palette(mid); border-radius: 10px; "
        "font-size: 18px; } QLineEdit:focus { border: 2px solid #007acc; outline: none; } "
        "QLineEdit::placeholder { font-size: 18px; color: palette(window-text); }");

    emailInput_ = std::make_unique<QLineEdit>();
    emailInput_->setPlaceholderText("Email address");
    emailInput_->setMinimumSize(300, 50);
    emailInput_->setMaximumSize(500, 65);
    emailInput_->setFont(inputFont);
    emailInput_->setStyleSheet("QLineEdit { background-color: palette(alternate-base); color: palette(text); padding: "
                               "12px 20px; border: 2px solid palette(mid); border-radius: 10px; "
                               "font-size: 18px; } QLineEdit:focus { border: 2px solid #007acc; outline: none; } "
                               "QLineEdit::placeholder { font-size: 18px; color: palette(window-text); }");

    passwordInput_ = std::make_unique<QLineEdit>();
    passwordInput_->setPlaceholderText("Password");
    passwordInput_->setEchoMode(QLineEdit::Password);
    passwordInput_->setMinimumSize(300, 50);
    passwordInput_->setMaximumSize(500, 65);
    passwordInput_->setFont(inputFont);
    passwordInput_->setStyleSheet(
        "QLineEdit { background-color: palette(alternate-base); color: palette(text); padding: 12px 50px 12px 20px; "
        "border: 2px solid palette(mid); border-radius: "
        "10px; font-size: 18px; } QLineEdit:focus { border: 2px solid #007acc; outline: "
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
    signUpButton_->setMinimumSize(300, 60);
    signUpButton_->setMaximumSize(500, 85);
    QFont buttonFont("Roboto", 22, QFont::Bold);
    signUpButton_->setFont(buttonFont);
    signUpButton_->setCursor(Qt::PointingHandCursor);
    signUpButton_->setObjectName("signUpButton");
}

void SignUpWindow::setupBackToLoginLink()
{
    backToLoginButton_ = std::make_unique<QPushButton>("Already have an account? Sign in");
    backToLoginButton_->setFlat(true);
    backToLoginButton_->setFont(QFont("Roboto", 16));
    backToLoginButton_->setCursor(Qt::PointingHandCursor);
    backToLoginButton_->setObjectName("backToLoginButton");
}

void SignUpWindow::setupLayout()
{
    mainLayout_ = std::make_unique<QVBoxLayout>(this);
    mainLayout_->setSpacing(0);
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->addWidget(customTitleBar_.get());

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background: transparent;");

    centerContainer_ = new QWidget();
    centerContainer_->setObjectName("centerContainer");
    centerContainer_->setStyleSheet("background: transparent;");
    auto centerLayout = new QVBoxLayout(centerContainer_);
    centerLayout->setAlignment(Qt::AlignCenter);
    centerLayout->setSpacing(25);
    centerLayout->setContentsMargins(40, 20, 40, 40);

    centerLayout->addStretch(1);
    centerLayout->addWidget(iconLabel_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(5);
    centerLayout->addWidget(titleLabel_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(usernameInput_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(15);
    centerLayout->addWidget(emailInput_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(15);

    auto passwordContainer = std::make_unique<QWidget>();
    passwordContainer->setMinimumSize(300, 50);
    passwordContainer->setMaximumSize(500, 65);
    passwordInput_->setParent(passwordContainer.get());
    passwordToggleButton_->setParent(passwordContainer.get());

    auto *pLayout = new QHBoxLayout(passwordContainer.get());
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(passwordInput_.get());
    passwordToggleButton_->raise();

    passwordContainer->installEventFilter(this);

    centerLayout->addWidget(passwordContainer.release(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(35);
    centerLayout->addWidget(signUpButton_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(25);
    centerLayout->addWidget(backToLoginButton_.get(), 0, Qt::AlignCenter);
    centerLayout->addStretch(1);

    centerContainer_->setLayout(centerLayout);
    scrollArea->setWidget(centerContainer_);
    mainLayout_->addWidget(scrollArea);
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

bool SignUpWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize || event->type() == QEvent::Move)
    {
        if (passwordInput_ && passwordToggleButton_ && passwordInput_->parentWidget())
        {
            QWidget *container = passwordInput_->parentWidget();
            passwordToggleButton_->move(container->width() - 42, (container->height() - 32) / 2);
        }
    }
    return QWidget::eventFilter(obj, event);
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
                    if (customTitleBar_)
                    {
                        customTitleBar_->setThemeService(themeService_);
                    }
                    update();
                });
        setupLogo();
        setupStyles();
        if (customTitleBar_)
        {
            customTitleBar_->setThemeService(themeService_);
        }
    }
}
