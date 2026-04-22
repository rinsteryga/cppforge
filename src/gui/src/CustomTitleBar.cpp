#include "CustomTitleBar.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QIcon>
#include <QFont>
#include <QDebug>
#include <QTimer> // Добавили для гарантированного срабатывания

CustomTitleBar::CustomTitleBar(QWidget *parent) : QWidget(parent)
{
    setupUI();
    if (parent)
    {
        parent->installEventFilter(this);
    }
}

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
    if (titleLabel_) titleLabel_->setText(title);
}

void CustomTitleBar::setIcon(const QIcon &icon)
{
    if (iconLabel_) iconLabel_->setPixmap(icon.pixmap(24, 24));
}

void CustomTitleBar::onMinimizeClicked()
{
    QWidget* win = window();
    if (!win) return;

    minimizeButton_->clearFocus();
    
    // Используем прямой метод Qt для сворачивания. 
    // Если простое win->showMinimized() не подхватывается, 
    // QTimer::singleShot гарантирует, что команда выполнится сразу после обработки клика.
    QTimer::singleShot(0, win, &QWidget::showMinimized);
}

void CustomTitleBar::onMaximizeRestoreClicked()
{
    QWidget* win = window();
    if (!win) return;

    if (win->isMaximized()) 
    {
        win->showNormal(); // Это вернет окно в нормальное состояние корректно
    } 
    else 
    {
        win->showMaximized(); // Это вызовет WindowStateChange, который подхватит MainWindow
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
        isResizing_ = false; // Сброс при каждом новом нажатии
        
        // Запоминаем позицию только если окно в нормальном состоянии
        if (!window()->isMaximized()) {
            dragPosition_ = event->globalPos() - window()->frameGeometry().topLeft();
        }
        event->accept();
    }
}

void CustomTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isResizing_ = false; // Гарантированный сброс блокировки
    }
    QWidget::mouseReleaseEvent(event);
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    // Если произошел даблклик и кнопка еще зажата — ничего не делаем
    if (isResizing_) {
        event->accept();
        return;
    }

    if (event->buttons() & Qt::LeftButton)
    {
        QWidget* win = window();
        if (!win) return;

        if (win->isMaximized()) 
        {
            // Механика "отрыва" окна от верхнего края
            double relativeX = (double)event->pos().x() / win->width();
            
            // Важно: блокируем сигналы или флаг на мгновение, чтобы смена состояния не вызвала скачок
            win->showNormal();

            int newX = event->globalPos().x() - (win->width() * relativeX);
            int newY = event->globalPos().y() - event->pos().y();
            
            // qMax(0, newY) не дает окну улететь под заголовок экрана
            win->move(newX, qMax(0, newY)); 
            
            // Пересчитываем dragPosition_, чтобы движение продолжалось плавно
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
        isResizing_ = true; // Блокируем MouseMove до отпускания кнопки
        onMaximizeRestoreClicked();
        event->accept();
    }
}

bool CustomTitleBar::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == window() && event->type() == QEvent::WindowStateChange)
    {
        if (maximizeRestoreButton_)
        {
            maximizeRestoreButton_->setText(window()->isMaximized() ? "❐" : "□");
        }
    }
    return QWidget::eventFilter(obj, event);
}