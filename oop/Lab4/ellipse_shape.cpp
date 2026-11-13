#include "ellipse_shape.h"

#include <QPainter>
#include <QPoint>
#include <QRect>

void EllipseShape::Show(QPainter& painter) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter.setBrush(Qt::NoBrush);
    EllipseShape::Trace(painter);
    painter.restore();
}

void EllipseShape::Trace(QPainter& painter) const {
    painter.drawEllipse(QRect(QPoint(x1_, y1_), QPoint(x2_, y2_)).normalized());
}
