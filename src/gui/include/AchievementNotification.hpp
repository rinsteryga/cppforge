#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>

namespace cppforge::gui
{

    class AchievementNotification : public QFrame
    {
        Q_OBJECT
    public:
        explicit AchievementNotification(const QString &name, const QString &desc, const QString &iconPath,
                                         QWidget *parent = nullptr);

        void showAnimated();

    private:
        void setupUI(const QString &name, const QString &desc, const QString &iconPath);
        void applyStyles();

        QPropertyAnimation *fadeAnimation = nullptr;
        QPropertyAnimation *moveAnimation = nullptr;
    };

} // namespace cppforge::gui
