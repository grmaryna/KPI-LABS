#pragma once

#include <QIcon>

#include "shape.h"

struct ShapeInfo {
    const char* id;
    const char* title;
    const char* tooltip;
    Shape* (*create)();
    int icon[4];
    int order;
};

template <class T>
Shape* MakeShape() {
    return new T;
}

struct ShapeRegistrar {
    explicit ShapeRegistrar(const ShapeInfo& info);
};

int ShapeKindCount();
const ShapeInfo& GetShapeInfo(int kind);
const ShapeInfo* FindShapeInfo(const char* id);
QIcon MakeShapeIcon(int kind);