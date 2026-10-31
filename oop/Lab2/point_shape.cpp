#include "point_shape.h"

#include <QPainter>

void PointShape::Show(QPainter& painter) const {
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPoint(x1_, y1_), 3, 3);
    painter.restore();
}

void PointShape::Trace(QPainter&) const {
}