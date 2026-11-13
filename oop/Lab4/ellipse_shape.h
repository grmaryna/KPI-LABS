#pragma once

#include "shape.h"

// Еліпс: ввід по двох протилежних кутах охоплюючого прямокутника.
// Відображення: чорний контур без заповнення.
class EllipseShape : public virtual Shape {
public:
    void Show(QPainter& painter) const override;
    void Trace(QPainter& painter) const override;
};
