#pragma once

#include "shape.h"

class RectShape : public virtual Shape {
public:
    void Show(QPainter& painter) const override;
    void Trace(QPainter& painter) const override;

    void SetFilled(bool filled) { filled_ = filled; }

private:
    bool filled_ = true;
};
