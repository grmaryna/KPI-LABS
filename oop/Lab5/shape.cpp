#include "shape.h"

void Shape::Set(int x1, int y1, int x2, int y2) {
    x1_ = x1;
    y1_ = y1;
    x2_ = x2;
    y2_ = y2;
}

QRect Shape::Bounds() const {
    return QRect(QPoint(x1_, y1_), QPoint(x2_, y2_)).normalized();
}