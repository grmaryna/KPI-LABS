#pragma once

#include "line_shape.h"
#include "rect_shape.h"

class CubeShape : public LineShape, public RectShape {
public:
    void Show(QPainter& painter) const override;
    void Trace(QPainter& painter) const override;

private:

    struct Faces {
        int fx, fy;
        int bx, by;
        int w, h;
    };
    Faces Layout() const;
};
