#include "CustomTitleBar.hpp"
#include <QApplication>
#include <QFont>
#include <QIcon>

CustomTitleBar::CustomTitleBar(QWidget *parent) : QWidget(parent)
{
    setupUI();
    if (parent) {
        window()->installEventFilter(this);
    }
}

void CustomTitleBar::setupUI()
{
    setFixedHeight(40);
    // ПРЯМЫЕ УГЛЫ: border-radius: 0px обязателен
    setStyleSheet("background-color: white; border-bottom: 1px solid #e0e0e0; border-radius: 0px;");

    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(10, 0, 0, 0);
    layout_->setSpacing(0);

    iconLabel_ = new QLabel(this);
    iconLabel_->setFixedSize(24, 24);
    iconLabel_->setScaledContents(true);
    iconLabel_->setStyleSheet("border: none; border-radius: 0px;");

    titleLabel_ = new QLabel(this);
    titleLabel_->setFont(QFont("Roboto", 10, QFont::Bold));
    titleLabel_->setStyleSheet("color: #000000; padding-left: 8px; border: none;");

    minimizeButton_ = new QPushButton("-", this);
    maximizeRestoreButton_ = new QPushButton("□", this);
    closeButton_ = new QPushButton("✕", this);

    // Убираем закругления у кнопок при наведении
    QString buttonStyle = "QPushButton { "
                          "background-color: transparent; border: none; font-size: 18px; "
                          "color: #5f6368; border-radius: 0px; "
                          "} "
                          "QPushButton:hover { background-color: #e8eaed; }";

    QString closeStyle = "QPushButton { "
                         "background-color: transparent; border: none; font-size: 18px; "
                         "color: #5f6368; border-radius: 0px; "
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

void CustomTitleBar::setTitle(const QString &title) { if (titleLabel_) titleLabel_->setText(title); }
void CustomTitleBar::setIcon(const QIcon &icon) { if (iconLabel_) iconLabel_->setPixmap(icon.pixmap(24, 24)); }

void CustomTitleBar::onMinimizeClicked() { window()->showMinimized(); }
void CustomTitleBar::onMaximizeRestoreClicked() {
    if (window()->isMaximized()) window()->showNormal();
    else window()->showMaximized();
}
void CustomTitleBar::onCloseClicked() { window()->close(); }

void CustomTitleBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPosition_ = event->globalPos() - window()->frameGeometry().topLeft();
        event->accept();
    }
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        window()->move(event->globalPos() - dragPosition_);
        event->accept();
    }
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        onMaximizeRestoreClicked();
        event->accept();
    }
}

bool CustomTitleBar::eventFilter(QObject *obj, QEvent *event) {
    if (obj == window() && event->type() == QEvent::WindowStateChange) {
        maximizeRestoreButton_->setText(window()->isMaximized() ? "❐" : "□");
    }
    return QWidget::eventFilter(obj, event);
}