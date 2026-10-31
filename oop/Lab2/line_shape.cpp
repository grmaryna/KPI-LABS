#include "line_shape.h"

#include <QPainter>

void LineShape::Show(QPainter& painter) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    Trace(painter);
    painter.restore();
}

void LineShape::Trace(QPainter& painter) const {
    painter.drawLine(x1_, y1_, x2_, y2_);
}