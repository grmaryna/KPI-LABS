#pragma once

#include "ellipse_shape.h"
#include "line_shape.h"

class LineOOShape : public LineShape, public EllipseShape {
public:
    void Show(QPainter& painter) const override;
    void Trace(QPainter& painter) const override;

private:
    static constexpr int kRadius = 6;
};