#include "point_shape.h"

#include <QPainter>
#include <QPoint>

#include "shape_registry.h"

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

QRect PointShape::Bounds() const {
    return QRect(x1_ - 2, y1_ - 2, 5, 5);
}

namespace {
const ShapeRegistrar kRegistrar({"point", "Крапка", "Крапка",
                                 &MakeShape<PointShape>, {16, 16, 16, 16}, 10});
}