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
        QSize targetSize = savedGeometry_.size();
        window->resize(clampSizeToScreen(targetSize));
    }
    else if (!hasState_)
    {
        window->resize(clampSizeToScreen(defaultSize));
    }

    centerOnScreen(window);
}

QSize WindowStateManager::clampSizeToScreen(const QSize &requestedSize)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen)
    {
        return requestedSize;
    }

    QRect available = screen->availableGeometry();
    int w = qMin(requestedSize.width(), available.width());
    int h = qMin(requestedSize.height(), available.height());

    return QSize(w, h);
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
        QSize finalSize = clampSizeToScreen(window->size());
        if (window->size() != finalSize)
        {
            window->resize(finalSize);
        }
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
