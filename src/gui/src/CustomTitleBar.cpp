#include "CustomTitleBar.hpp"

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
    setStyleSheet("background-color: white; border-bottom: 1px solid #e0e0e0;");

    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(10, 0, 0, 0);
    layout_->setSpacing(0);

    iconLabel_ = new QLabel(this);
    iconLabel_->setFixedSize(24, 24);
    iconLabel_->setScaledContents(true);

    titleLabel_ = new QLabel(this);
    titleLabel_->setFont(QFont("Roboto", 10, QFont::Bold));
    titleLabel_->setStyleSheet("color: #000000; padding-left: 8px; border: none;");

    minimizeButton_ = new QPushButton("-", this);
    maximizeRestoreButton_ = new QPushButton("□", this);
    closeButton_ = new QPushButton("✕", this);

    const QString buttonStyle = "QPushButton { "
                                "background-color: transparent; border: none; font-size: 18px; "
                                "color: #5f6368; "
                                "} "
                                "QPushButton:hover { background-color: #e8eaed; }";

    const QString closeStyle = "QPushButton { "
                               "background-color: transparent; border: none; font-size: 18px; "
                               "color: #5f6368; "
                               "} "
                               "QPushButton:hover { background-color: #e81123; color: white; }";

    minimizeButton_->setFixedSize(60, 40);
    minimizeButton_->setStyleSheet(buttonStyle);
    maximizeRestoreButton_->setFixedSize(60, 40);
    maximizeRestoreButton_->setStyleSheet(buttonStyle);
    closeButton_->setFixedSize(60, 40);
    closeButton_->setStyleSheet(closeStyle);

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
    {
        event->accept();
        return;
    }

    if (event->buttons() & Qt::LeftButton)
    {
        QWidget *win = window();
        if (!win)
            return;

        if (win->isMaximized())
        {
            double relativeX = (double)event->pos().x() / win->width();

            win->showNormal();

            int newX = event->globalPos().x() - (win->width() * relativeX);
            int newY = event->globalPos().y() - event->pos().y();

            win->move(newX, qMax(0, newY));

            dragPosition_ = event->globalPos() - win->frameGeometry().topLeft();
        }
        else
        {
            win->move(event->globalPos() - dragPosition_);
        }
        event->accept();
    }
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        onMaximizeRestoreClicked();
        event->accept();
    }
}

bool CustomTitleBar::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == window())
    {
        if (event->type() == QEvent::WindowStateChange)
        {
            updateButtonIcons();

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
