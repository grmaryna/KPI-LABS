#pragma once

#include <QIcon>

#include "shape.h"

// Таблиця типів фігур. Щоб додати нову фігуру, достатньо написати її клас
// і додати один рядок у таблицю в shape_registry.cpp — меню, Toolbar,
// заголовок вікна та редактор підхоплять її автоматично.
struct ShapeInfo {
    const char* title;     // назва в меню та в заголовку вікна (UTF-8)
    const char* tooltip;   // підказка (tooltip) кнопки Toolbar (UTF-8)
    Shape* (*create)();    // фабрика об'єкта фігури
    int icon[4];           // координати фігури на іконці 32x32
};

int ShapeKindCount();
const ShapeInfo& GetShapeInfo(int kind);
QIcon MakeShapeIcon(int kind);
