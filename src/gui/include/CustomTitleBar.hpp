#pragma once

#include <QIcon>
#include <QPoint>
#include <QString>
#include <QWidget>

class QHBoxLayout;
class QLabel;
class QPushButton;
class QMouseEvent;

namespace cppforge::services
{
    class ThemeService;
}

/**
 * @class CustomTitleBar
 * @brief A custom title bar widget for frameless windows.
 *
 * Provides standard window controls (minimize, maximize/restore, close)
 * and handles window dragging and state changes.
 */
class CustomTitleBar : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a CustomTitleBar.
     * @param parent The parent window (must be the window this title bar controls).
     */
    explicit CustomTitleBar(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~CustomTitleBar() override;

    /**
     * @brief Sets the text displayed in the title bar.
     * @param title The new title text.
     */
    void setTitle(const QString &title);

    /**
     * @brief Sets the icon displayed in the title bar.
     * @param icon The new icon.
     */
    void setIcon(const QIcon &icon);

    /**
     * @brief Sets the theme service and connects to its signals.
     * @param service Pointer to the ThemeService.
     */
    void setThemeService(cppforge::services::ThemeService *service);

protected:
    /**
     * @brief Handles mouse press events for dragging initiation.
     * @param event The mouse event.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Handles mouse move events for window dragging.
     * @param event The mouse event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Handles double-click events for maximize/restore toggle.
     * @param event The mouse event.
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    /**
     * @brief Handles mouse release events.
     * @param event The mouse event.
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

    /**
     * @brief Filters events from the parent window to sync state.
     * @param obj The object being filtered.
     * @param event The event.
     * @return true if the event should be filtered, false otherwise.
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    /**
     * @brief Minimizes the window.
     */
    void onMinimizeClicked();

    /**
     * @brief Toggles between maximized and normal state.
     */
    void onMaximizeRestoreClicked();

    /**
     * @brief Closes the window.
     */
    void onCloseClicked();

private:
    /**
     * @brief Initializes the user interface components.
     */
    void setupUI();

    /**
     * @brief Updates the icons and state of buttons.
     */
    void updateButtonIcons();

    /**
     * @brief Updates the stylesheet based on the current theme.
     */
    void updateStyles();

    QLabel *iconLabel_ = nullptr;                  ///< Label for the window icon.
    QLabel *titleLabel_ = nullptr;                 ///< Label for the window title text.
    QPushButton *minimizeButton_ = nullptr;        ///< The minimize button.
    QPushButton *maximizeRestoreButton_ = nullptr; ///< The maximize/restore button.
    QPushButton *closeButton_ = nullptr;           ///< The close button.
    QHBoxLayout *layout_ = nullptr;                ///< Main layout for the title bar.

    cppforge::services::ThemeService *themeService_ = nullptr; ///< Reference to the theme service.

    QPoint dragPosition_;     ///< Stores the starting position for window dragging.
    bool isResizing_ = false; ///< Internal flag to prevent dragging during resize-like actions.
};