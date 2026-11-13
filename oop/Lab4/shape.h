#pragma once

class QPainter;

// Абстрактний базовий клас усіх фігур.
// Фігури, які беруть участь у множинному успадкуванні (LineShape, RectShape,
// EllipseShape), успадковують Shape ВІРТУАЛЬНО, тому в складених фігурах
// (лінія з кружечками, каркас куба) існує лише один підоб'єкт Shape.
class Shape {
public:
    Shape() = default;
    Shape(const Shape&) = default;
    Shape& operator=(const Shape&) = default;
    virtual ~Shape() = default;

    void Set(int x1, int y1, int x2, int y2);

    virtual void Show(QPainter& painter) const = 0;   // остаточне зображення
    virtual void Trace(QPainter& painter) const = 0;  // «гумовий» слід

protected:
    int x1_ = 0;
    int y1_ = 0;
    int x2_ = 0;
    int y2_ = 0;
};
