#pragma once

#include "shape.h"

class LineShape : public virtual Shape {
public:
    void Show(QPainter& painter) const override;
    void Trace(QPainter& painter) const override;
};
