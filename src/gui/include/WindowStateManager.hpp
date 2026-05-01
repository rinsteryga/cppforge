#pragma once

#include <QRect>
#include <QSize>

class QWidget;

/**
 * @brief Singleton that centralizes window geometry and maximized state.
 *
 * When any managed window changes its state (maximized, normal, or resized),
 * the manager remembers it. When a new managed window is about to be shown,
 * the manager applies the last known state so all windows share the same
 * screen layout. The state is persisted via QSettings.
 */
class WindowStateManager
{
public:
    /**
     * @brief Returns the global singleton instance.
     * @return Reference to the WindowStateManager.
     */
    static WindowStateManager &instance();

    /**
     * @brief Saves the current window geometry and maximized flag as the shared state.
     * @param window The window whose state should be captured.
     */
    void captureState(QWidget *window);

    /**
     * @brief Applies the full shared state to the given window and shows it.
     *
     * If the stored state is maximized the window is shown maximized.
     * Otherwise the saved size is applied, the window is centred on the
     * primary screen and shown in normal mode. If no state exists the
     * window is resized to @p defaultSize and centred.
     *
     * @param window The target window to show.
     * @param defaultSize Fallback size when no state exists.
     */
    void applyState(QWidget *window, const QSize &defaultSize = QSize(1000, 700));

    /**
     * @brief Applies only the saved geometry without showing the window.
     *
     * Sets the window size (from the saved state or @p defaultSize) and
     * centres it on the primary screen. The window visibility is not changed.
     * Use this in window constructors where the window must not become visible.
     *
     * @param window The target window to configure.
     * @param defaultSize Fallback size when no state exists.
     */
    void applyGeometry(QWidget *window, const QSize &defaultSize = QSize(1000, 700));

    /**
     * @brief Checks whether the stored state is maximized.
     * @return True if the last captured state was maximized.
     */
    bool isMaximized() const;

    /**
     * @brief Checks whether any state has been stored yet.
     * @return True if captureState() has been called or a persisted state was loaded.
     */
    bool hasStoredState() const;

    /**
     * @brief Returns the stored window geometry rectangle.
     * @return The last captured geometry; invalid if no state exists.
     */
    QRect storedGeometry() const;

private:
    /**
     * @brief Private constructor for singleton pattern.
     */
    WindowStateManager();

    /**
     * @brief Private destructor.
     */
    ~WindowStateManager();

    WindowStateManager(const WindowStateManager &) = delete;
    WindowStateManager &operator=(const WindowStateManager &) = delete;

    /**
     * @brief Centers the window on the primary screen.
     * @param window The window to center.
     */
    void centerOnScreen(QWidget *window);

    /**
     * @brief Saves the current state to persistent storage.
     */
    void persistState();

    /**
     * @brief Loads the state from persistent storage.
     */
    void loadState();

    bool hasState_{false};    ///< Flag indicating if a state has been loaded or captured.
    bool isMaximized_{false}; ///< Whether the last captured state was maximized.
    QRect savedGeometry_;     ///< The last captured normal window geometry.
};
