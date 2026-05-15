#include "../include/AchievementNotification.hpp"

#include <QApplication>
#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QScreen>
#include <QTimer>

namespace cppforge::gui
{
    AchievementNotification::AchievementNotification(const QString &name, const QString &desc, const QString &iconPath,
                                                     QWidget *parent)
        : QFrame(parent)
    {
        qDebug() << "[AchievementNotification] Creating notification for:" << name;
        setupUI(name, desc, iconPath);
        applyStyles();

        setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_ShowWithoutActivating);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_DeleteOnClose, true);
    }

    void AchievementNotification::setupUI(const QString &name, const QString &desc, const QString &iconPath)
    {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(15, 15, 15, 15);

        auto *contentFrame = new QFrame();
        contentFrame->setObjectName("contentFrame");
        mainLayout->addWidget(contentFrame);

        auto *layout = new QHBoxLayout(contentFrame);
        layout->setContentsMargins(15, 10, 15, 10);
        layout->setSpacing(15);

        auto *iconLabel = new QLabel();
        iconLabel->setFixedSize(50, 50);
        QPixmap pix(iconPath);
        if (pix.isNull())
        {
            iconLabel->setText("🏆");
            iconLabel->setAlignment(Qt::AlignCenter);
            iconLabel->setStyleSheet("font-size: 24px; background: rgba(255,255,255,0.1); border-radius: 8px;");
        }
        else
        {
            iconLabel->setPixmap(pix.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            iconLabel->setAlignment(Qt::AlignCenter);
        }
        layout->addWidget(iconLabel);

        auto *textLayout = new QVBoxLayout();
        auto *titleLabel = new QLabel("ACHIEVEMENT UNLOCKED");
        titleLabel->setStyleSheet("color: palette(highlight); font-weight: 900; font-size: 10px; letter-spacing: 1px;");

        auto *nameLabel = new QLabel(name);
        nameLabel->setObjectName("nameLabel");

        auto *descLabel = new QLabel(desc);
        descLabel->setObjectName("descLabel");
        descLabel->setWordWrap(true);
        descLabel->setMaximumHeight(40);

        textLayout->addWidget(titleLabel);
        textLayout->addWidget(nameLabel);
        textLayout->addWidget(descLabel);
        layout->addLayout(textLayout);

        setFixedSize(420, 150);

        auto *shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(20);
        shadow->setColor(QColor(0, 0, 0, 180));
        shadow->setOffset(0, 5);
        contentFrame->setGraphicsEffect(shadow);
    }

    void AchievementNotification::applyStyles()
    {
        bool isDark = (palette().color(QPalette::Window).lightness() < 128);
        QString bgColor = isDark ? "rgba(20, 20, 25, 230)" : "rgba(255, 255, 255, 240)";
        QString borderColor = isDark ? "palette(highlight)" : "#444444";
        QString nameColor = isDark ? "white" : "black";
        QString descColor = isDark ? "#bbbbbb" : "#555555";

        setStyleSheet(QString(R"(
        QFrame#contentFrame {
            background-color: %1;
            border: 3px solid %2;
            border-radius: 12px;
        }
        QLabel { background: transparent; border: none; }
        QLabel#nameLabel { color: %3; font-weight: bold; font-size: 15px; }
        QLabel#descLabel { color: %4; font-size: 12px; }
    )")
                          .arg(bgColor)
                          .arg(borderColor)
                          .arg(nameColor)
                          .arg(descColor));
    }

    void AchievementNotification::showAnimated()
    {
        qDebug() << "[AchievementNotification] Starting animation...";

        QRect anchorGeom;
        if (parentWidget())
        {
            anchorGeom = parentWidget()->window()->geometry();
        }
        else
        {
            QScreen *screen = QApplication::primaryScreen();
            if (screen)
                anchorGeom = screen->availableGeometry();
            else
                return;
        }

        int startX = anchorGeom.right() - width() - 20;
        int startY = anchorGeom.bottom();
        int endY = anchorGeom.bottom() - height() - 20;

        setWindowOpacity(0.0);
        move(startX, startY);
        show();

        fadeAnimation = new QPropertyAnimation(this, "windowOpacity");
        fadeAnimation->setDuration(600);
        fadeAnimation->setStartValue(0.0);
        fadeAnimation->setEndValue(1.0);
        fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);

        moveAnimation = new QPropertyAnimation(this, "pos");
        moveAnimation->setDuration(600);
        moveAnimation->setStartValue(QPoint(startX, startY));
        moveAnimation->setEndValue(QPoint(startX, endY));
        moveAnimation->setEasingCurve(QEasingCurve::OutCubic);

        fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        moveAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        QTimer::singleShot(4000, this,
                           [this, startX, endY]()
                           {
                               auto *fadeOut = new QPropertyAnimation(this, "windowOpacity");
                               fadeOut->setDuration(600);
                               fadeOut->setStartValue(1.0);
                               fadeOut->setEndValue(0.0);

                               auto *moveOut = new QPropertyAnimation(this, "pos");
                               moveOut->setDuration(600);
                               moveOut->setStartValue(QPoint(startX, endY));
                               moveOut->setEndValue(QPoint(startX, endY + 30));
                               moveOut->setEasingCurve(QEasingCurve::InCubic);

                               connect(fadeOut, &QPropertyAnimation::finished, this, &QWidget::deleteLater);
                               fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
                               moveOut->start(QAbstractAnimation::DeleteWhenStopped);
                           });
    }
} // namespace cppforge::gui
