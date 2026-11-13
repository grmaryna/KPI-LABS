#pragma once

#include "shape.h"

// Прямокутник: ввід від центру до одного з кутів.
// Відображення: чорний контур, світло-зелене заповнення (за замовчуванням).
class RectShape : public virtual Shape {
public:
    void Show(QPainter& painter) const override;
    void Trace(QPainter& painter) const override;

    // Вимкнення заповнення потрібне для каркасу куба.
    void SetFilled(bool filled) { filled_ = filled; }

private:
    bool filled_ = true;
};
