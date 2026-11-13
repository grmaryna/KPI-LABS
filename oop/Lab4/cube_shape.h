#pragma once

#include "line_shape.h"
#include "rect_shape.h"

// Каркас куба — множинне успадкування:
//   дві грані (передня й задня)  -> RectShape::Show
//   чотири бічні ребра           -> LineShape::Show
// Ввід: по двох протилежних кутах охоплюючого прямокутника всього куба.
class CubeShape : public LineShape, public RectShape {
public:
    void Show(QPainter& painter) const override;
    void Trace(QPainter& painter) const override;

private:
    // Координати кутів двох граней, обчислені з охоплюючого прямокутника.
    struct Faces {
        int fx, fy;  // лівий верхній кут передньої грані
        int bx, by;  // лівий верхній кут задньої грані
        int w, h;    // розміри кожної грані (парні — для точного збігу кутів)
    };
    Faces Layout() const;
};
