#pragma once

#include "shape.h"

class PointShape : public Shape {
public:
    void Show(QPainter& painter) const override;
    void Trace(QPainter& painter) const override;
    const char* Id() const override { return "point"; }
    QRect Bounds() const override;
};