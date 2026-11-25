#pragma once

#include <QRect>

class QPainter;

class Shape {
public:
    Shape() = default;
    Shape(const Shape&) = default;
    Shape& operator=(const Shape&) = default;
    virtual ~Shape() = default;

    void Set(int x1, int y1, int x2, int y2);
    int X1() const { return x1_; }
    int Y1() const { return y1_; }
    int X2() const { return x2_; }
    int Y2() const { return y2_; }

    virtual void Show(QPainter& painter) const = 0;
    virtual void Trace(QPainter& painter) const = 0;

    virtual const char* Id() const = 0;

    virtual QRect Bounds() const;

protected:
    int x1_ = 0;
    int y1_ = 0;
    int x2_ = 0;
    int y2_ = 0;
};