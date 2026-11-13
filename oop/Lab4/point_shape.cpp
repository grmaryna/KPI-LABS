#include "point_shape.h"

#include <QPainter>
#include <QPoint>

void PointShape::Show(QPainter& painter) const {
    painter.save();
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    PointShape::Trace(painter);
    painter.restore();
}

void PointShape::Trace(QPainter& painter) const {
    painter.drawEllipse(QPoint(x1_, y1_), 2, 2);
}
