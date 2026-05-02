#pragma once

#include "../../core/include/services/ThemeService.hpp"

#include <QObject>

namespace cppforge::gui
{

    /**
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
         * @brief Applies the specified theme globally.
         * @param theme The theme to apply (Light or Dark).
         */
        void applyTheme(cppforge::services::Theme theme);

    private:
        cppforge::services::ThemeService *m_themeService;

        void applyDarkTheme();
        void applyLightTheme();
        void updateWidgetStyle(QWidget *widget);
    };

} // namespace cppforge::gui
