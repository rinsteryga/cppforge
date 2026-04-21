#pragma once

#include <QPoint>
#include <QString>
#include <QWidget>

#include <vector>

struct RoadmapNode
{
    int lessonId;
    QString title;
    bool isCompleted;
    bool isLocked;
    QPoint pos;
};

class ModuleRoadmapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleRoadmapWidget(QWidget *parent = nullptr);

    void setLessons(const std::vector<RoadmapNode> &nodes);

    int getFirstIncompleteY() const;

signals:
    void lessonSelected(int lessonId);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    std::vector<RoadmapNode> m_nodes;

    const int m_nodeRadius = 40;
    const int m_spacing = 140;
    int m_hoveredNodeIndex = -1;

    void updateLayout();
};