#pragma once

#include <QObject>

namespace cppforge::services
{

    /**
     * @brief Enum representing the available visual themes in the application.
     */
    enum class Theme
    {
        Light,
        Dark
    };

    /**
     * @brief Service responsible for managing and persisting the application's visual theme state.
     *
     * This class loads the user's theme preference from QSettings and provides
     * signals when the theme changes so the GUI can react accordingly.
     */
    class ThemeService : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Constructor that initializes the theme service and loads saved preferences.
         * @param parent The parent QObject.
         */
        explicit ThemeService(QObject *parent = nullptr);

        /**
         * @brief Gets the current theme.
         * @return The currently active Theme.
         */
        Theme getCurrentTheme() const;

        /**
         * @brief Sets the new theme, persists it to settings, and emits a signal.
         * @param theme The new theme to apply.
         */
        void setTheme(Theme theme);

    signals:
        /**
         * @brief Signal emitted when the theme has changed.
         * @param newTheme The newly applied theme.
         */
        void themeChanged(Theme newTheme);

    private:
        Theme m_currentTheme;
    };

} // namespace cppforge::services
