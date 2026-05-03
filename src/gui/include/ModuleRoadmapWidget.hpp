#pragma once

#include "../../core/include/entities/RoadmapNodeData.hpp"

#include <QPoint>
#include <QString>
#include <QWidget>

#include <vector>

namespace cppforge::services
{
    class ThemeService;
}

/**
 * @struct RoadmapNode
 * @brief Representation of a node in the roadmap.
 */
struct RoadmapNode
{
    cppforge::entities::RoadmapNodeData data; ///< Core node data.
    QPoint pos;                               ///< Visual position on the widget.
};

/**
 * @class ModuleRoadmapWidget
 * @brief A custom-painted widget that displays a roadmap of lessons.
 */
class ModuleRoadmapWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a ModuleRoadmapWidget.
     * @param parent Parent widget.
     */
    explicit ModuleRoadmapWidget(QWidget *parent = nullptr);

    /**
     * @brief Sets the lessons to be displayed.
     * @param nodes Vector of roadmap nodes.
     */
    void setLessons(const std::vector<RoadmapNode> &nodes);

    /**
     * @brief Gets the Y-coordinate of the first incomplete lesson.
     * @return Y-coordinate.
     */
    int getFirstIncompleteY() const;

    /**
     * @brief Sets the theme service and connects to its signals.
     * @param service Pointer to the ThemeService.
     */
    void setThemeService(cppforge::services::ThemeService *service);

signals:
    /**
     * @brief Emitted when a lesson node is selected.
     * @param lessonId ID of the selected lesson.
     */
    void lessonSelected(int lessonId);

protected:
    /**
     * @brief Paints the roadmap nodes and paths.
     * @param event Paint event.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Handles widget resizing and updates node positions.
     * @param event Resize event.
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * @brief Handles mouse press for node selection.
     * @param event Mouse event.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Handles mouse move for hover effects.
     * @param event Mouse event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Resets hover state when mouse leaves the widget.
     * @param event Leave event.
     */
    void leaveEvent(QEvent *event) override;

private:
    /**
     * @brief Recalculates the positions of all nodes.
     */
    void updateLayout();

    std::vector<RoadmapNode> m_nodes;                           ///< List of nodes.
    cppforge::services::ThemeService *m_themeService = nullptr; ///< Theme service.
    const int m_nodeRadius = 40;                                ///< Radius of each node.
    const int m_spacing = 140;                                  ///< Vertical spacing between nodes.
    int m_hoveredNodeIndex = -1;                                ///< Index of the currently hovered node.
};