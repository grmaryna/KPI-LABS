#pragma once

#include <QIcon>

#include "shape.h"

struct ShapeInfo {
    const char* title;
    const char* tooltip;
    Shape* (*create)();
    int icon[4];
};

int ShapeKindCount();
const ShapeInfo& GetShapeInfo(int kind);
QIcon MakeShapeIcon(int kind);
