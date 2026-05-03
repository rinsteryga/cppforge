#include "../include/WindowStateManager.hpp"

#include <QGuiApplication>
#include <QScreen>
#include <QSettings>
#include <QWidget>

WindowStateManager &WindowStateManager::instance()
{
    static WindowStateManager mgr;
    return mgr;
}

WindowStateManager::WindowStateManager()
{
    loadState();
}

WindowStateManager::~WindowStateManager() = default;

void WindowStateManager::captureState(QWidget *window)
{
    if (!window)
    {
        return;
    }

    Qt::WindowStates state = window->windowState();

    if (!(state & Qt::WindowMinimized))
    {
        isMaximized_ = (state & Qt::WindowMaximized);
    }

    if (!isMaximized_ && state == Qt::WindowNoState)
    {
        savedGeometry_ = window->geometry();
    }

    hasState_ = true;
    persistState();
}

void WindowStateManager::applyState(QWidget *window, const QSize &defaultSize)
{
    if (!window)
    {
        return;
    }

    loadState();

    if (hasState_ && isMaximized_)
    {
        window->showMaximized();
    }
    else
    {
        applyGeometry(window, defaultSize);
        window->showNormal();
    }
}

void WindowStateManager::applyGeometry(QWidget *window, const QSize &defaultSize)
{
    if (!window)
    {
        return;
    }

    loadState();

    if (hasState_ && !isMaximized_)
    {
        window->resize(savedGeometry_.size());
    }
    else if (!hasState_)
    {
        window->resize(defaultSize);
    }

    centerOnScreen(window);
}

bool WindowStateManager::isMaximized() const
{
    return isMaximized_;
}

bool WindowStateManager::hasStoredState() const
{
    return hasState_;
}

QRect WindowStateManager::storedGeometry() const
{
    return savedGeometry_;
}

void WindowStateManager::centerOnScreen(QWidget *window)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QRect geo = screen->availableGeometry();
        window->move(geo.center() - window->rect().center());
    }
}

void WindowStateManager::persistState()
{
    QSettings settings("CppForge", "StudyApp");
    settings.beginGroup("SharedWindowState");
    settings.setValue("geometry", savedGeometry_);
    settings.setValue("isMaximized", isMaximized_);
    settings.endGroup();
    settings.sync();
}

void WindowStateManager::loadState()
{
    QSettings settings("CppForge", "StudyApp");
    settings.sync();
    settings.beginGroup("SharedWindowState");

    if (settings.contains("geometry"))
    {
        savedGeometry_ = settings.value("geometry").toRect();
        isMaximized_ = settings.value("isMaximized", false).toBool();
        hasState_ = true;
    }

    settings.endGroup();
}
