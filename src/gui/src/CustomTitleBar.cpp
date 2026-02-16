#include "CustomTitleBar.hpp"
#include <QStyle>
#include <QApplication>
#include <QFont>

CustomTitleBar::CustomTitleBar(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void CustomTitleBar::setupUI() {
    setFixedHeight(40);
    setStyleSheet("background-color: white; border-bottom: 1px solid #e0e0e0;"); 

    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(10, 0, 0, 0);
    layout_->setSpacing(0);

    iconLabel_ = new QLabel(this);
    iconLabel_->setFixedSize(24, 24);
    iconLabel_->setScaledContents(true);

    titleLabel_ = new QLabel(this);
    QFont titleFont("Roboto", 10, QFont::Bold);
    titleLabel_->setFont(titleFont);
    titleLabel_->setStyleSheet("color: #000000; padding-left: 8px;");

    minimizeButton_ = new QPushButton("-", this);
    maximizeRestoreButton_ = new QPushButton("□", this);
    closeButton_ = new QPushButton("✕", this);

    QString buttonStyle = 
        "QPushButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "   color: #5f6368;"
        "}"
        "QPushButton:hover {"
        "   background-color: #e8eaed;"
        "   color: #202124;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #dadce0;"
        "   color: #202124;"
        "}";

    QString closeButtonStyle = 
        "QPushButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "   color: #5f6368;"
        "}"
        "QPushButton:hover {"
        "   background-color: #e81123;" 
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #b71c1c;"
        "   color: white;"
        "}";

    minimizeButton_->setFixedSize(45, 40);
    minimizeButton_->setStyleSheet(buttonStyle);

    maximizeRestoreButton_->setFixedSize(45, 40);
    maximizeRestoreButton_->setStyleSheet(buttonStyle);

    closeButton_->setFixedSize(45, 40);
    closeButton_->setStyleSheet(closeButtonStyle);

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

void CustomTitleBar::setTitle(const QString &title) {
    titleLabel_->setText(title);
}

void CustomTitleBar::setIcon(const QIcon &icon) {
    iconLabel_->setPixmap(icon.pixmap(24, 24));
}

void CustomTitleBar::onMinimizeClicked() {
    if (parentWidget()) {
        parentWidget()->showMinimized();
    }
}

void CustomTitleBar::onMaximizeRestoreClicked() {
    if (parentWidget()) {
        if (parentWidget()->isMaximized()) {
            parentWidget()->showNormal();
            maximizeRestoreButton_->setText("□");
        } else {
            parentWidget()->showMaximized();
            maximizeRestoreButton_->setText("❐");
        }
    }
}

void CustomTitleBar::onCloseClicked() {
    if (parentWidget()) {
        parentWidget()->close();
    }
}

void CustomTitleBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPosition_ = event->globalPos() - parentWidget()->frameGeometry().topLeft();
        event->accept();
    }
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        if (parentWidget()) {
             parentWidget()->move(event->globalPos() - dragPosition_);
             event->accept();
        }
    }
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        onMaximizeRestoreClicked();
        event->accept();
    }
}
