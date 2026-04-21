#include "ModuleRoadmapWidget.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include <cmath>

ModuleRoadmapWidget::ModuleRoadmapWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(450);
}

void ModuleRoadmapWidget::updateLayout()
{
    if (m_nodes.empty())
        return;

    int spacing = 140;
    int totalHeight = (m_nodes.size() + 1) * spacing;
    setMinimumHeight(totalHeight);

    for (size_t i = 0; i < m_nodes.size(); ++i)
    {
        int y = totalHeight - ((i + 1) * spacing);

        int x = width() / 2 + std::sin(i * 1.0) * (width() * 0.2);

        m_nodes[i].pos = QPoint(x, y);
    }
    update();
}

void ModuleRoadmapWidget::setLessons(const std::vector<RoadmapNode> &nodes)
{
    m_nodes = nodes;
    updateLayout();
}

void ModuleRoadmapWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateLayout();
}

void ModuleRoadmapWidget::paintEvent(QPaintEvent *event)
{
    if (m_nodes.empty())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.moveTo(m_nodes[0].pos);
    for (size_t i = 1; i < m_nodes.size(); ++i)
    {
        QPoint p1 = m_nodes[i - 1].pos;
        QPoint p2 = m_nodes[i].pos;
        int controlY = (p1.y() + p2.y()) / 2;
        path.cubicTo(QPoint(p1.x(), controlY), QPoint(p2.x(), controlY), p2);
    }

    painter.setPen(QPen(QColor("#E5E5E5"), 12, Qt::SolidLine, Qt::RoundCap));
    painter.drawPath(path);

    for (size_t i = 0; i < m_nodes.size(); ++i)
    {
        const auto &node = m_nodes[i];

        QColor mainColor =
            node.isLocked ? QColor("#E5E5E5") : (node.isCompleted ? QColor("#58CC02") : QColor("#1CB0F6"));
        QColor shadowColor = mainColor.darker(120);

        painter.setBrush(shadowColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(node.pos.x() - m_nodeRadius, node.pos.y() - m_nodeRadius + 5, m_nodeRadius * 2,
                            m_nodeRadius * 2);

        painter.setBrush(mainColor);
        painter.drawEllipse(node.pos.x() - m_nodeRadius, node.pos.y() - m_nodeRadius, m_nodeRadius * 2,
                            m_nodeRadius * 2);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Roboto", 12, QFont::Bold));
        QString icon = node.isCompleted ? "✔" : (node.isLocked ? "🔒" : "▶");
        painter.drawText(QRect(node.pos.x() - 15, node.pos.y() - 15, 30, 30), Qt::AlignCenter, icon);

        painter.setPen(QColor("#3C3C3C"));
        painter.setFont(QFont("Roboto", 12, QFont::Bold));

        QString label = QString("№%1").arg(i + 1);
        int textOffset = (node.pos.x() > width() / 2) ? -85 : 55;

        QRect textRect(node.pos.x() + textOffset, node.pos.y() - 15, 50, 30);
        painter.drawText(textRect, Qt::AlignCenter, label);

        if (!node.isCompleted && !node.isLocked)
        {
            painter.setPen(QColor("#1CB0F6"));
            painter.setFont(QFont("Roboto", 9, QFont::Black));
            painter.drawText(node.pos.x() + textOffset, node.pos.y() + 15, 50, 20, Qt::AlignCenter, "START");
        }
    }
}

void ModuleRoadmapWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        for (const auto &node : m_nodes)
        {
            int dx = event->pos().x() - node.pos.x();
            int dy = event->pos().y() - node.pos.y();
            if ((dx * dx + dy * dy) <= (m_nodeRadius * m_nodeRadius))
            {
                if (!node.isLocked)
                {
                    emit lessonSelected(node.lessonId);
                }
                return;
            }
        }
    }
}
