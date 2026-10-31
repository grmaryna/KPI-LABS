#pragma once

class QPainter;

class Shape {
public:
    virtual ~Shape() = default;

    void Set(int x1, int y1, int x2, int y2);

    virtual void Show(QPainter& painter) const = 0;
    virtual void Trace(QPainter& painter) const = 0;

protected:
    int x1_ = 0;
    int y1_ = 0;
    int x2_ = 0;
    int y2_ = 0;
};