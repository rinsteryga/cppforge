#include "AuthWindow.hpp"

#include "../../core/include/services/SessionManager.hpp"
#include "SignUpWindow.hpp"
#include "WindowStateManager.hpp"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QFont>
#include <QGuiApplication>
#include <QIcon>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
#include <QScrollArea>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QStyleOption>
#include <QTimer>

AuthWindow::AuthWindow(std::shared_ptr<cppforge::services::AuthManager> authManager, QWidget *parent)
    : QWidget(parent), signUpWindow_(nullptr), passwordVisible_(false), authManager_(authManager)
{
    setupUI();
    setWindowOpacity(0.0);

    WindowStateManager::instance().applyGeometry(this, QSize(1280, 900));

    QTimer::singleShot(100, this, &AuthWindow::fadeIn);
}

AuthWindow::~AuthWindow() = default;

void AuthWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void AuthWindow::setupWindowProperties()
{
    setMinimumSize(600, 500);
    setWindowTitle("cppforge Log in");
    setWindowIcon(QIcon(":/icons/main_logo_pale.ico"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setObjectName("AuthWindow");
    setupStyles();
}

void AuthWindow::setupStyles()
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
        #AuthWindow, #centerContainer { background-color: palette(window); border: none; }
        
        QPushButton#loginButton { 
            background-color: %1; 
            color: white; 
            border-radius: 12px; 
            font-size: 24px; 
            font-weight: bold; 
            border: none; 
        }
        QPushButton#loginButton:hover { background-color: %2; color: %3; }
        
        QPushButton#createAccountButton { 
            color: %4; 
            font-size: 18px; 
            background: transparent; 
            border: none; 
            border-bottom: 1px solid transparent; 
            padding: 15px; 
        }
        QPushButton#createAccountButton:hover { color: %2; border-bottom: 1px solid %2; }
        
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

void AuthWindow::setupTitleBar()
{
    customTitleBar_ = std::make_unique<CustomTitleBar>(this);
    customTitleBar_->setTitle("Log in - cppforge");
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
    connect(usernameInput_.get(), &QLineEdit::returnPressed, this, &AuthWindow::onLoginClicked);
    connect(passwordInput_.get(), &QLineEdit::returnPressed, this, &AuthWindow::onLoginClicked);
}

void AuthWindow::centerWindow()
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

void AuthWindow::fadeIn()
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

void AuthWindow::setupLogo()
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

void AuthWindow::showFallbackLogo()
{
    iconLabel_->setStyleSheet("background-color: transparent; border: none;");
    iconLabel_->setText("");
}

void AuthWindow::setupTitle()
{
    titleLabel_ = std::make_unique<QLabel>("Log Into cppforge");
    titleLabel_->setFont(QFont("Roboto", 32, QFont::Bold));
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setStyleSheet("color: palette(text); padding: 10px;");
}

void AuthWindow::setupInputFields()
{
    usernameInput_ = std::make_unique<QLineEdit>();
    usernameInput_->setPlaceholderText("Username or email address");
    usernameInput_->setMinimumSize(300, 50);
    usernameInput_->setMaximumSize(500, 65);
    usernameInput_->setStyleSheet("QLineEdit { background-color: palette(alternate-base); color: palette(text); "
                                  "padding: 12px 20px; border: 2px solid palette(mid); border-radius: 10px; "
                                  "font-size: 18px; } QLineEdit:focus { border: 2px solid #007acc; }");

    passwordInput_ = std::make_unique<QLineEdit>();
    passwordInput_->setPlaceholderText("Password");
    passwordInput_->setEchoMode(QLineEdit::Password);
    passwordInput_->setMinimumSize(300, 50);
    passwordInput_->setMaximumSize(500, 65);
    passwordInput_->setStyleSheet("QLineEdit { background-color: palette(alternate-base); color: palette(text); "
                                  "padding: 12px 50px 12px 20px; border: 2px solid palette(mid); border-radius: "
                                  "10px; font-size: 18px; } QLineEdit:focus { border: 2px solid #007acc; }");

    passwordToggleButton_ = std::make_unique<QPushButton>();
    passwordToggleButton_->setFixedSize(32, 32);
    passwordToggleButton_->setCursor(Qt::PointingHandCursor);
    passwordToggleButton_->setStyleSheet("background: transparent; border: none;");
    QPixmap eye(":/images/eye_open.png");
    if (!eye.isNull())
        passwordToggleButton_->setIcon(QIcon(eye.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

void AuthWindow::togglePasswordVisibility()
{
    passwordVisible_ = !passwordVisible_;
    passwordInput_->setEchoMode(passwordVisible_ ? QLineEdit::Normal : QLineEdit::Password);
    QPixmap eye(passwordVisible_ ? ":/images/eye_slash.png" : ":/images/eye_open.png");
    if (!eye.isNull())
        passwordToggleButton_->setIcon(QIcon(eye.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

void AuthWindow::setupLoginButton()
{
    loginButton_ = std::make_unique<QPushButton>("Log in");
    loginButton_->setMinimumSize(300, 60);
    loginButton_->setMaximumSize(500, 85);
    loginButton_->setCursor(Qt::PointingHandCursor);
    loginButton_->setObjectName("loginButton");
}

void AuthWindow::setupCreateAccountLink()
{
    createAccountButton_ = std::make_unique<QPushButton>("New to cppforge? Create an account");
    createAccountButton_->setFlat(true);
    createAccountButton_->setCursor(Qt::PointingHandCursor);
    createAccountButton_->setObjectName("createAccountButton");
}

void AuthWindow::setupLayout()
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(customTitleBar_.get());

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background: transparent;");

    centerContainer_ = new QWidget();
    centerContainer_->setObjectName("centerContainer");
    centerContainer_->setStyleSheet("background: transparent;");
    auto *centerLayout = new QVBoxLayout(centerContainer_);
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

    auto *passwordContainer = new QWidget(centerContainer_);
    passwordContainer->setMinimumSize(300, 50);
    passwordContainer->setMaximumSize(500, 65);
    passwordInput_->setParent(passwordContainer);
    passwordToggleButton_->setParent(passwordContainer);

    auto *pLayout = new QHBoxLayout(passwordContainer);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(passwordInput_.get());
    passwordToggleButton_->raise();

    auto updatePasswordTogglePos = [this, passwordContainer]()
    { passwordToggleButton_->move(passwordContainer->width() - 42, (passwordContainer->height() - 32) / 2); };
    passwordContainer->installEventFilter(this);

    centerLayout->addWidget(passwordContainer, 0, Qt::AlignCenter);
    centerLayout->addSpacing(35);
    centerLayout->addWidget(loginButton_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(25);
    centerLayout->addWidget(createAccountButton_.get(), 0, Qt::AlignCenter);
    centerLayout->addStretch(1);

    scrollArea->setWidget(centerContainer_);
    layout->addWidget(scrollArea);
}

void AuthWindow::onLoginClicked()
{
    QString username = usernameInput_->text().trimmed();
    QString password = passwordInput_->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "Login Failed", "Please fill in all fields.");
        return;
    }

    if (authManager_ && authManager_->login(username, password))
    {
        int userId = -1;
        auto currentUser = cppforge::services::SessionManager::instance().getCurrentUser();
        if (currentUser)
        {
            userId = static_cast<int>(currentUser->getId());
        }

        QSettings settings("CppForge", "StudyApp");
        settings.setValue("auth/remember", true);
        settings.setValue("auth/user_id", userId);
        settings.setValue("auth/username", usernameInput_->text());

        if (transitionAnimation_)
            transitionAnimation_->stop();

        WindowStateManager::instance().captureState(this);
        this->hide();
        emit switchToMainMenu(usernameInput_->text(), userId);
    }
    else
    {
        QMessageBox::warning(this, "Login Failed", "Invalid credentials.");
    }
}

void AuthWindow::onCreateAccountClicked()
{
    openSignUpWindow();
}

void AuthWindow::openSignUpWindow()
{
    if (!signUpWindow_)
    {
        signUpWindow_ = std::make_unique<SignUpWindow>(authManager_);
        if (themeService_)
        {
            signUpWindow_->setThemeService(themeService_);
        }
        connect(signUpWindow_.get(), &SignUpWindow::switchToLogin,
                [this]()
                {
                    WindowStateManager::instance().applyState(this, QSize(1280, 900));
                    this->fadeIn();
                });
    }
    WindowStateManager::instance().captureState(this);
    this->hide();
    WindowStateManager::instance().applyState(signUpWindow_.get(), QSize(1280, 900));
    signUpWindow_->fadeIn();
}

bool AuthWindow::eventFilter(QObject *obj, QEvent *event)
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

void AuthWindow::setThemeService(cppforge::services::ThemeService *service)
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
                    if (signUpWindow_)
                    {
                        signUpWindow_->setThemeService(themeService_);
                    }
                    update();
                });

        setupLogo();
        setupStyles();
        if (customTitleBar_)
        {
            customTitleBar_->setThemeService(themeService_);
        }
        if (signUpWindow_)
        {
            signUpWindow_->setThemeService(themeService_);
        }
    }
}
void AuthWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        qApp->quit();
    }
    QWidget::keyPressEvent(event);
}
