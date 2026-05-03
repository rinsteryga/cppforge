#include "ThemeManager.hpp"

#include <QApplication>
#include <QIcon>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QWidget>

namespace cppforge::gui
{

    ThemeManager::ThemeManager(cppforge::services::ThemeService *themeService, QObject *parent)
        : QObject(parent), m_themeService(themeService)
    {
        if (m_themeService)
        {
            connect(m_themeService, &cppforge::services::ThemeService::themeChanged, this, &ThemeManager::applyTheme);
            applyTheme(m_themeService->getCurrentTheme());
        }
    }

    void ThemeManager::applyTheme(cppforge::services::Theme theme)
    {
        if (theme == cppforge::services::Theme::Dark)
        {
            applyDarkTheme();
        }
        else
        {
            applyLightTheme();
        }

        QString scrollStyle = R"(
            QScrollBar:vertical {
                border: none;
                background: transparent;
                width: 8px;
                margin: 0px;
            }
            QScrollBar::handle:vertical {
                background: #888;
                min-height: 20px;
                border-radius: 4px;
            }
            QScrollBar::handle:vertical:hover {
                background: #aaa;
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0px;
            }
            QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
                background: transparent;
            }
            QScrollBar:horizontal {
                border: none;
                background: transparent;
                height: 8px;
                margin: 0px;
            }
            QScrollBar::handle:horizontal {
                background: #888;
                min-width: 20px;
                border-radius: 4px;
            }
            QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
                width: 0px;
            }
        )";

        qApp->setStyleSheet(scrollStyle);

        for (QWidget *widget : QApplication::topLevelWidgets())
        {
            updateWidgetStyle(widget);
        }
    }

    void ThemeManager::updateWidgetStyle(QWidget *widget)
    {
        if (!widget)
            return;

        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
        widget->update();

        for (QObject *child : widget->children())
        {
            if (QWidget *childWidget = qobject_cast<QWidget *>(child))
            {
                updateWidgetStyle(childWidget);
            }
        }
    }

    void ThemeManager::applyDarkTheme()
    {
        qApp->setStyle(QStyleFactory::create("Fusion"));

        QColor darkBackground(28, 28, 28);
        QColor lighterDark(39, 39, 42);
        QColor accentColor(155, 156, 217);

        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, darkBackground);
        darkPalette.setColor(QPalette::WindowText, QColor(244, 244, 245));
        darkPalette.setColor(QPalette::Base, lighterDark);
        darkPalette.setColor(QPalette::AlternateBase, darkBackground);
        darkPalette.setColor(QPalette::ToolTipBase, QColor(63, 63, 70));
        darkPalette.setColor(QPalette::ToolTipText, QColor(244, 244, 245));
        darkPalette.setColor(QPalette::Text, QColor(244, 244, 245));
        darkPalette.setColor(QPalette::Button, lighterDark);
        darkPalette.setColor(QPalette::ButtonText, QColor(244, 244, 245));
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Mid, QColor(63, 63, 70));
        darkPalette.setColor(QPalette::Midlight, QColor(39, 39, 42));
        darkPalette.setColor(QPalette::Shadow, Qt::black);
        darkPalette.setColor(QPalette::Highlight, accentColor);
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);

        darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(161, 161, 170));
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(161, 161, 170));
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(161, 161, 170));
        darkPalette.setColor(QPalette::Disabled, QPalette::Button, QColor(63, 63, 70));

        qApp->setPalette(darkPalette);
    }

    void ThemeManager::applyLightTheme()
    {
        qApp->setStyle(QStyleFactory::create("Fusion"));

        QPalette lightPalette;
        lightPalette.setColor(QPalette::Window, QColor(245, 247, 251));
        lightPalette.setColor(QPalette::WindowText, QColor(33, 37, 41));
        lightPalette.setColor(QPalette::Base, Qt::white);
        lightPalette.setColor(QPalette::AlternateBase, QColor(245, 247, 251));
        lightPalette.setColor(QPalette::ToolTipBase, Qt::white);
        lightPalette.setColor(QPalette::ToolTipText, Qt::black);
        lightPalette.setColor(QPalette::Text, Qt::black);
        lightPalette.setColor(QPalette::Button, QColor(98, 99, 155));
        lightPalette.setColor(QPalette::ButtonText, Qt::white);
        lightPalette.setColor(QPalette::BrightText, Qt::red);
        lightPalette.setColor(QPalette::Mid, QColor(211, 215, 220));
        lightPalette.setColor(QPalette::Midlight, QColor(245, 247, 251));
        lightPalette.setColor(QPalette::Shadow, QColor(0, 0, 0, 30));
        lightPalette.setColor(QPalette::Highlight, QColor(98, 99, 155));
        lightPalette.setColor(QPalette::HighlightedText, Qt::white);

        lightPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(153, 153, 153));
        lightPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(153, 153, 153));
        lightPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(153, 153, 153));
        lightPalette.setColor(QPalette::Disabled, QPalette::Button, QColor(240, 240, 240));

        qApp->setPalette(lightPalette);
    }

} // namespace cppforge::gui
