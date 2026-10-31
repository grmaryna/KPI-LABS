#include "ellipse_shape.h"

#include <QPainter>
#include <QPoint>
#include <cstdlib>

void EllipseShape::Show(QPainter& painter) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter.setBrush(Qt::white);
    Trace(painter);
    painter.restore();
}

void EllipseShape::Trace(QPainter& painter) const {
    const int rx = std::abs(x2_ - x1_);
    const int ry = std::abs(y2_ - y1_);
    painter.drawEllipse(QPoint(x1_, y1_), rx, ry);
}