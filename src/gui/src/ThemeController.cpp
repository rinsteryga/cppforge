#include "ThemeController.hpp"

#include <QSettings>

namespace cppforge::gui
{
    ThemeController::ThemeController(QObject *parent)
        : QObject(parent)
    {
        QSettings settings("CppForge", "StudyApp");
        int savedTheme = settings.value("app/theme", 0).toInt();
        m_isDark = (savedTheme == 1);
    }

    ThemeController *ThemeController::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
    {
        Q_UNUSED(qmlEngine);
        Q_UNUSED(jsEngine);
        return new ThemeController();
    }

    void ThemeController::setIsDark(bool dark)
    {
        if (m_isDark == dark)
        {
            return;
        }

        m_isDark = dark;

        QSettings settings("CppForge", "StudyApp");
        settings.setValue("app/theme", m_isDark ? 1 : 0);
        settings.sync();

        emit isDarkChanged(m_isDark);
    }

    void ThemeController::toggleTheme()
    {
        setIsDark(!m_isDark);
    }
} // namespace cppforge::gui
