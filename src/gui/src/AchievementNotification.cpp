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
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(15, 15, 15, 15);
        layout->setSpacing(15);

        auto *iconLabel = new QLabel();
        iconLabel->setFixedSize(60, 60);
        QPixmap pix(iconPath);
        if (pix.isNull())
        {
            iconLabel->setText("🏆");
            iconLabel->setAlignment(Qt::AlignCenter);
            iconLabel->setStyleSheet("font-size: 30px; background: #333; border-radius: 5px;");
        }
        else
        {
            iconLabel->setPixmap(pix.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        layout->addWidget(iconLabel);

        auto *textLayout = new QVBoxLayout();
        auto *titleLabel = new QLabel("Достижение получено!");
        titleLabel->setStyleSheet("color: #62639b; font-weight: bold; font-size: 12px;");
        auto *nameLabel = new QLabel(name);
        nameLabel->setStyleSheet("color: white; font-weight: bold; font-size: 16px;");
        auto *descLabel = new QLabel(desc);
        descLabel->setStyleSheet("color: #ccc; font-size: 13px;");
        descLabel->setWordWrap(true);

        descLabel->setWordWrap(true);
        descLabel->setMaximumHeight(45);

        textLayout->addWidget(titleLabel);
        textLayout->addWidget(nameLabel);
        textLayout->addWidget(descLabel);
        layout->addLayout(textLayout);

        setFixedSize(400, 140);
    }

    void AchievementNotification::applyStyles()
    {
        setStyleSheet(R"(
        QFrame {
            background-color: #1a1a1a;
            border: 2px solid #62639b;
            border-radius: 10px;
        }
    )");
    }

    void AchievementNotification::showAnimated()
    {
        qDebug() << "[AchievementNotification] Starting animation...";

        QRect anchorGeom;
        if (parentWidget())
        {
            anchorGeom = parentWidget()->window()->geometry();
            qDebug() << "[AchievementNotification] Using parent window geometry:" << anchorGeom;
        }
        else
        {
            QScreen *screen = QApplication::primaryScreen();
            if (screen)
            {
                anchorGeom = screen->availableGeometry();
                qDebug() << "[AchievementNotification] No parent, using screen geometry:" << anchorGeom;
            }
            else
            {
                return;
            }
        }

        int startX = anchorGeom.right() - width() - 20;
        int startY = anchorGeom.bottom();
        int endY = anchorGeom.bottom() - height() - 20;

        qDebug() << "[AchievementNotification] Calculated Pos:" << startX << endY;

        move(startX, startY);
        show();

        auto *eff = new QGraphicsOpacityEffect(this);
        eff->setOpacity(0.0);
        setGraphicsEffect(eff);

        fadeAnimation = new QPropertyAnimation(eff, "opacity");
        fadeAnimation->setDuration(500);
        fadeAnimation->setStartValue(0.0);
        fadeAnimation->setEndValue(1.0);

        moveAnimation = new QPropertyAnimation(this, "pos");
        moveAnimation->setDuration(500);
        moveAnimation->setStartValue(QPoint(startX, startY));
        moveAnimation->setEndValue(QPoint(startX, endY));
        moveAnimation->setEasingCurve(QEasingCurve::OutCubic);

        fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        moveAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        QTimer::singleShot(5000, this,
                           [this, eff, startX, endY]()
                           {
                               qDebug() << "[AchievementNotification] Starting fade out...";
                               auto *fadeOut = new QPropertyAnimation(eff, "opacity");
                               fadeOut->setDuration(500);
                               fadeOut->setStartValue(1.0);
                               fadeOut->setEndValue(0.0);

                               auto *moveOut = new QPropertyAnimation(this, "pos");
                               moveOut->setDuration(500);
                               moveOut->setStartValue(QPoint(startX, endY));
                               moveOut->setEndValue(QPoint(startX, endY + 20));

                               connect(fadeOut, &QPropertyAnimation::finished, this, &QWidget::deleteLater);
                               fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
                               moveOut->start(QAbstractAnimation::DeleteWhenStopped);
                           });
    }
} // namespace cppforge::gui
