#include "services/ThemeService.hpp"

#include <QSettings>

namespace cppforge::services
{

    ThemeService::ThemeService(QObject *parent) : QObject(parent), m_currentTheme(Theme::Light)
    {
        QSettings settings("CppForge", "StudyApp");
        int savedTheme = settings.value("app/theme", static_cast<int>(Theme::Light)).toInt();

        if (savedTheme == static_cast<int>(Theme::Dark))
        {
            m_currentTheme = Theme::Dark;
        }
    }

    Theme ThemeService::getCurrentTheme() const
    {
        return m_currentTheme;
    }

    void ThemeService::setTheme(Theme theme)
    {
        if (m_currentTheme == theme)
            return;

        m_currentTheme = theme;

        QSettings settings("CppForge", "StudyApp");
        settings.setValue("app/theme", static_cast<int>(theme));
        settings.sync();

        emit themeChanged(m_currentTheme);
    }

} // namespace cppforge::services
