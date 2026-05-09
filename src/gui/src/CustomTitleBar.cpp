#include "CustomTitleBar.hpp"

#include "../../core/include/services/ThemeService.hpp"
#include "WindowStateManager.hpp"

#include <QApplication>
#include <QDebug>
#include <QFont>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QScreen>
#include <QSettings>
#include <QTimer>

CustomTitleBar::CustomTitleBar(QWidget *parent) : QWidget(parent)
{
    setupUI();
    if (parent)
    {
        parent->installEventFilter(this);
    }
}

CustomTitleBar::~CustomTitleBar() = default;

void CustomTitleBar::setupUI()
{
    setFixedHeight(40);
    setStyleSheet("background-color: palette(window); border-bottom: 1px solid; border-bottom-color: palette(mid); "
                  "border-top-left-radius: 19px; border-top-right-radius: 19px;");

    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(15, 0, 0, 0);
    layout_->setSpacing(10);

    iconLabel_ = new QLabel(this);
    iconLabel_->setFixedSize(22, 22);
    iconLabel_->setScaledContents(true);
    iconLabel_->setStyleSheet("background: transparent; border: none;");

    titleLabel_ = new QLabel(this);
    titleLabel_->setStyleSheet("color: palette(text); font-weight: bold; font-family: 'Roboto'; font-size: 13px; "
                               "background: transparent; border: none;");

    minimizeButton_ = new QPushButton("-", this);
    maximizeRestoreButton_ = new QPushButton("□", this);
    closeButton_ = new QPushButton("✕", this);

    updateStyles();

    minimizeButton_->setFixedSize(50, 40);
    maximizeRestoreButton_->setFixedSize(50, 40);
    closeButton_->setFixedSize(50, 40);

    layout_->addWidget(iconLabel_);
    layout_->addWidget(titleLabel_);
    layout_->addStretch();
    layout_->addWidget(minimizeButton_);
    layout_->addWidget(maximizeRestoreButton_);
    layout_->addWidget(closeButton_);

    connect(minimizeButton_, &QPushButton::clicked, this, &CustomTitleBar::onMinimizeClicked);
    connect(maximizeRestoreButton_, &QPushButton::clicked, this, &CustomTitleBar::onMaximizeRestoreClicked);
    connect(closeButton_, &QPushButton::clicked, this, &CustomTitleBar::onCloseClicked);
}

void CustomTitleBar::setTitle(const QString &title)
{
    if (titleLabel_)
        titleLabel_->setText(title);
}

void CustomTitleBar::setIcon(const QIcon &icon)
{
    if (iconLabel_)
        iconLabel_->setPixmap(icon.pixmap(24, 24));
}

void CustomTitleBar::onMinimizeClicked()
{
    QWidget *win = window();
    if (!win)
        return;

    minimizeButton_->clearFocus();

    QTimer::singleShot(0, win, &QWidget::showMinimized);
}

void CustomTitleBar::onMaximizeRestoreClicked()
{
    QWidget *win = window();
    if (!win)
        return;

    if (win->isMaximized())
    {
        win->showNormal();
    }
    else
    {
        win->showMaximized();
    }
}

void CustomTitleBar::onCloseClicked()
{
    window()->close();
}

void CustomTitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isResizing_ = false;

        if (!window()->isMaximized())
        {
            dragPosition_ = event->globalPos() - window()->frameGeometry().topLeft();
        }
        event->accept();
    }
}

void CustomTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isResizing_ = false;
        if (!window()->isMaximized() && !window()->isMinimized())
        {
            WindowStateManager::instance().captureState(window());
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (isResizing_)
        return;

    if (event->buttons() & Qt::LeftButton)
    {
        if (window()->isMaximized())
        {
            return;
        }

        window()->move(event->globalPos() - dragPosition_);
        event->accept();
    }
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        onMaximizeRestoreClicked();
    }
}

bool CustomTitleBar::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == window())
    {
        if (event->type() == QEvent::WindowStateChange)
        {
            updateButtonIcons();
        }
        else if (event->type() == QEvent::Resize || event->type() == QEvent::Move)
        {
            QTimer::singleShot(0, this,
                               [this]()
                               {
                                   if (window())
                                   {
                                       WindowStateManager::instance().captureState(window());
                                   }
                               });
        }
    }
    return QWidget::eventFilter(obj, event);
}

void CustomTitleBar::updateButtonIcons()
{
    if (maximizeRestoreButton_ && window())
    {
        maximizeRestoreButton_->setText(window()->isMaximized() ? "❐" : "□");
    }
}

void CustomTitleBar::setThemeService(cppforge::services::ThemeService *service)
{
    themeService_ = service;
    if (themeService_)
    {
        connect(themeService_, &cppforge::services::ThemeService::themeChanged, this, &CustomTitleBar::updateStyles);
        updateStyles();
    }
}

void CustomTitleBar::updateStyles()
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

    QString hoverColor = isDark ? "#0e639c" : "#62639b";
    QString hoverText = "white";

    const QString buttonStyle = QString("QPushButton { border: none; background: transparent; color: "
                                        "palette(window-text); font-size: 16px; } "
                                        "QPushButton:hover { background-color: %1; color: %2; border-radius: 0; }")
                                    .arg(hoverColor)
                                    .arg(hoverText);

    const QString closeStyle = "QPushButton { border: none; background: transparent; color: palette(window-text); "
                               "font-size: 16px; } "
                               "QPushButton:hover { background-color: #e81123; color: white; border-radius: 0; }";

    if (minimizeButton_)
    {
        minimizeButton_->setStyleSheet(buttonStyle);
        minimizeButton_->update();
    }
    if (maximizeRestoreButton_)
    {
        maximizeRestoreButton_->setStyleSheet(buttonStyle);
        maximizeRestoreButton_->update();
    }
    if (closeButton_)
    {
        closeButton_->setStyleSheet(closeStyle);
        closeButton_->update();
    }
}
