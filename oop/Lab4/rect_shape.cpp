#include "rect_shape.h"

#include <QPainter>
#include <cstdlib>

void RectShape::Show(QPainter& painter) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter.setBrush(filled_ ? QBrush(QColor(144, 238, 144)) : QBrush(Qt::NoBrush));
    RectShape::Trace(painter);
    painter.restore();
}

void RectShape::Trace(QPainter& painter) const {
    const int dx = std::abs(x2_ - x1_);
    const int dy = std::abs(y2_ - y1_);
    painter.drawRect(x1_ - dx, y1_ - dy, 2 * dx, 2 * dy);
}
