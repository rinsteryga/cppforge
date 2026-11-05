#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSettings>

namespace cppforge::gui
{
    /**
     * @class ThemeController
     * @brief Manages theme switching and persistence via QSettings.
     */
    class ThemeController : public QObject
    {
        Q_OBJECT
        QML_ELEMENT
        QML_SINGLETON

        Q_PROPERTY(bool isDark READ isDark WRITE setIsDark NOTIFY isDarkChanged)

    public:
        explicit ThemeController(QObject *parent = nullptr);
        ~ThemeController() override = default;

        static ThemeController *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

        bool isDark() const { return m_isDark; }
        void setIsDark(bool dark);

        Q_INVOKABLE void toggleTheme();

    signals:
        void isDarkChanged(bool dark);

    private:
        bool m_isDark = false;
    };
} // namespace cppforge::gui
