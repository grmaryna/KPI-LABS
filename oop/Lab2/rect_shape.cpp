#include "rect_shape.h"

#include <QPainter>
#include <QRect>

void RectShape::Show(QPainter& painter) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter.setBrush(QColor(255, 255, 0));
    Trace(painter);
    painter.restore();
}

void RectShape::Trace(QPainter& painter) const {
    painter.drawRect(QRect(QPoint(x1_, y1_), QPoint(x2_, y2_)).normalized());
}