#pragma once

#include "../../core/include/services/ThemeService.hpp"

#include <QObject>

class QWidget;

namespace cppforge::gui
{
    /**
     * @class ThemeManager
     * @brief Manager responsible for applying the visual theme across the GUI.
     *
     * This class listens to the ThemeService and applies the appropriate QPalette
     * and global QSS to the QApplication when the theme changes.
     */
    class ThemeManager : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Constructs the ThemeManager and applies the initial theme.
         * @param themeService Pointer to the ThemeService that stores current theme state.
         * @param parent The parent QObject.
         */
        explicit ThemeManager(cppforge::services::ThemeService *themeService, QObject *parent = nullptr);

        /**
         * @brief Destructor.
         */
        ~ThemeManager() override = default;

        /**
         * @brief Applies the specified theme globally.
         * @param theme The theme to apply (Light or Dark).
         */
        void applyTheme(cppforge::services::Theme theme);

    private:
        /**
         * @brief Applies the Dark theme palette and styles.
         */
        void applyDarkTheme();

        /**
         * @brief Applies the Light theme palette and styles.
         */
        void applyLightTheme();

        /**
         * @brief Updates specific widget properties during theme change.
         * @param widget The widget to update.
         */
        void updateWidgetStyle(QWidget *widget);

        cppforge::services::ThemeService *m_themeService; ///< Pointer to the theme service.
    };

} // namespace cppforge::gui
