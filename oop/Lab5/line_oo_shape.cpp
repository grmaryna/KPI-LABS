#include "line_oo_shape.h"

#include "shape_parts.h"
#include "shape_registry.h"

void LineOOShape::Show(QPainter& painter) const {
    LineShape::Show(painter);
    ShowPart<EllipseShape>(*this, painter,
                           x1_ - kRadius, y1_ - kRadius,
                           x1_ + kRadius, y1_ + kRadius);
    ShowPart<EllipseShape>(*this, painter,
                           x2_ - kRadius, y2_ - kRadius,
                           x2_ + kRadius, y2_ + kRadius);
}

void LineOOShape::Trace(QPainter& painter) const {
    LineShape::Trace(painter);
    TracePart<EllipseShape>(*this, painter,
                            x1_ - kRadius, y1_ - kRadius,
                            x1_ + kRadius, y1_ + kRadius);
    TracePart<EllipseShape>(*this, painter,
                            x2_ - kRadius, y2_ - kRadius,
                            x2_ + kRadius, y2_ + kRadius);
}

QRect LineOOShape::Bounds() const {
    return Shape::Bounds().adjusted(-kRadius, -kRadius, kRadius, kRadius);
}

namespace {
const ShapeRegistrar kRegistrar({"line_oo", "Лінія з кружечками", "Лінія з кружечками",
                                 &MakeShape<LineOOShape>, {8, 24, 24, 8}, 50});
}