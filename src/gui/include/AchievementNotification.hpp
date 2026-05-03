#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>

namespace cppforge::gui
{
    /**
     * @class AchievementNotification
     * @brief A floating notification window that appears when an achievement is unlocked.
     *
     * It features an animated slide-in and fade-in effect from the bottom-right
     * corner of the parent window or screen.
     */
    class AchievementNotification : public QFrame
    {
        Q_OBJECT
    public:
        /**
         * @brief Constructs an AchievementNotification.
         * @param name The name of the achievement.
         * @param desc A short description of the achievement.
         * @param iconPath Path to the achievement icon resource.
         * @param parent The parent widget (used for relative positioning).
         */
        explicit AchievementNotification(const QString &name, const QString &desc, const QString &iconPath,
                                         QWidget *parent = nullptr);

        /**
         * @brief Triggers the notification's slide-in and fade-in animation.
         */
        void showAnimated();

    private:
        /**
         * @brief Initializes the UI components for the notification.
         * @param name Achievement name.
         * @param desc Achievement description.
         * @param iconPath Icon resource path.
         */
        void setupUI(const QString &name, const QString &desc, const QString &iconPath);

        /**
         * @brief Applies the notification's stylesheet.
         */
        void applyStyles();

        QPropertyAnimation *fadeAnimation = nullptr; ///< Animation for opacity changes.
        QPropertyAnimation *moveAnimation = nullptr; ///< Animation for positional shifts.
    };

} // namespace cppforge::gui
