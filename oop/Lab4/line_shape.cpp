#include "line_shape.h"

#include <QPainter>

void LineShape::Show(QPainter& painter) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    // Кваліфікований виклик: малюється саме лінія, навіть якщо Show викликано
    // для складеної фігури, яка перевизначила Trace.
    LineShape::Trace(painter);
    painter.restore();
}

void LineShape::Trace(QPainter& painter) const {
    painter.drawLine(x1_, y1_, x2_, y2_);
}
