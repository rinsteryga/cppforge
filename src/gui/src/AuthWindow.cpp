#include "AuthWindow.hpp"

#include "SignUpWindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QFont>
#include <QGuiApplication>
#include <QIcon>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
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
    QTimer::singleShot(50, this, &AuthWindow::centerWindow);
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
    setFixedSize(1280, 900);
    setWindowTitle("cppforge Log in");
    setWindowIcon(QIcon(":/icons/main_logo.ico"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("AuthWindow");
    setStyleSheet("#AuthWindow { background-color: white; border: 1px solid #cccccc; }");
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
    iconLabel_ = std::make_unique<QLabel>();
    iconLabel_->setAlignment(Qt::AlignCenter);
    QPixmap logoPixmap(":/icons/main_logo.ico");
    if (!logoPixmap.isNull())
    {
        logoPixmap = logoPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel_->setPixmap(logoPixmap);
        iconLabel_->setFixedSize(200, 165);
    }
    else
    {
        showFallbackLogo();
    }
}

void AuthWindow::showFallbackLogo()
{
    iconLabel_->setFixedSize(200, 200);
    iconLabel_->setStyleSheet(
        "background-color: #4285f4; border-radius: 20px; color: white; font-size: 48px; font-weight: bold;");
    iconLabel_->setText("C++");
    iconLabel_->setAlignment(Qt::AlignCenter);
}

void AuthWindow::setupTitle()
{
    titleLabel_ = std::make_unique<QLabel>("Log Into cppforge");
    titleLabel_->setFont(QFont("Roboto", 32, QFont::Bold));
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setStyleSheet("color: #000000; padding: 10px;");
}

void AuthWindow::setupInputFields()
{
    usernameInput_ = std::make_unique<QLineEdit>();
    usernameInput_->setPlaceholderText("Username or email address");
    usernameInput_->setFixedSize(500, 65);
    usernameInput_->setStyleSheet("QLineEdit { padding: 18px 20px; border: 2px solid #cccccc; border-radius: 10px; "
                                  "font-size: 18px; } QLineEdit:focus { border: 3px solid #4285f4; }");

    passwordInput_ = std::make_unique<QLineEdit>();
    passwordInput_->setPlaceholderText("Password");
    passwordInput_->setEchoMode(QLineEdit::Password);
    passwordInput_->setFixedSize(500, 65);
    passwordInput_->setStyleSheet("QLineEdit { padding: 18px 50px 18px 20px; border: 2px solid #cccccc; border-radius: "
                                  "10px; font-size: 18px; } QLineEdit:focus { border: 3px solid #4285f4; }");

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
    loginButton_->setFixedSize(500, 85);
    loginButton_->setCursor(Qt::PointingHandCursor);
    loginButton_->setStyleSheet(
        "QPushButton { background-color: #62639b; color: white; border-radius: 12px; font-size: 24px; font-weight: "
        "bold; } QPushButton:hover { background-color: #7677B3; }");
}

void AuthWindow::setupCreateAccountLink()
{
    createAccountButton_ = std::make_unique<QPushButton>("New to cppforge? Create an account");
    createAccountButton_->setFlat(true);
    createAccountButton_->setCursor(Qt::PointingHandCursor);
    createAccountButton_->setStyleSheet("QPushButton { color: #4285f4; font-size: 18px; background: transparent; } "
                                        "QPushButton:hover { text-decoration: underline; }");
}

void AuthWindow::setupLayout()
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(customTitleBar_.get());

    auto *centerContainer = new QWidget(this);
    auto *centerLayout = new QVBoxLayout(centerContainer);
    centerLayout->setAlignment(Qt::AlignCenter);
    centerLayout->setSpacing(25);

    centerLayout->addWidget(iconLabel_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(5);
    centerLayout->addWidget(titleLabel_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(40);
    centerLayout->addWidget(usernameInput_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(20);

    auto *passwordContainer = new QWidget(centerContainer);
    passwordContainer->setFixedSize(500, 65);
    passwordInput_->setParent(passwordContainer);
    passwordInput_->setGeometry(0, 0, 500, 65);
    passwordToggleButton_->setParent(passwordContainer);
    passwordToggleButton_->setGeometry(500 - 42, 16, 32, 32);

    centerLayout->addWidget(passwordContainer, 0, Qt::AlignCenter);
    centerLayout->addSpacing(40);
    centerLayout->addWidget(loginButton_.get(), 0, Qt::AlignCenter);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(createAccountButton_.get(), 0, Qt::AlignCenter);

    layout->addWidget(centerContainer);
}

void AuthWindow::onLoginClicked()
{
    if (authManager_ && authManager_->login(usernameInput_->text(), passwordInput_->text()))
    {
        int userId = -1;
        QSqlQuery query;
        query.prepare("SELECT id FROM users WHERE username = :username");
        query.bindValue(":username", usernameInput_->text());

        if (query.exec() && query.next())
        {
            userId = query.value(0).toInt();
        }

        QSettings settings("CppForge", "StudyApp");
        settings.setValue("auth/remember", true);
        settings.setValue("auth/user_id", userId);
        settings.setValue("auth/username", usernameInput_->text());

        if (transitionAnimation_)
            transitionAnimation_->stop();

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
        connect(signUpWindow_.get(), &SignUpWindow::switchToLogin,
                [this]()
                {
                    this->show();
                    this->fadeIn();
                });
    }
    this->hide();
    signUpWindow_->show();
    signUpWindow_->fadeIn();
}