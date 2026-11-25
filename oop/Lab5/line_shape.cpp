#include "line_shape.h"

#include <QPainter>

#include "shape_registry.h"

void LineShape::Show(QPainter& painter) const {
    painter.save();
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));

    LineShape::Trace(painter);
    painter.restore();
}

void LineShape::Trace(QPainter& painter) const {
    painter.drawLine(x1_, y1_, x2_, y2_);
}

namespace {
const ShapeRegistrar kRegistrar({"line", "Лінія", "Лінія",
                                 &MakeShape<LineShape>, {4, 28, 28, 4}, 20});
}