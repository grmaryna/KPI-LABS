#pragma once

#include <QPoint>

#include "shape.h"

class QPainter;

class MyEditor {
public:
    static constexpr int N = 107;

    MyEditor();
    ~MyEditor();
    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;

    void SetShapeKind(int kind);
    int ShapeKind() const;
    int Count() const;

    bool OnLBdown(const QPoint& pos);
    bool OnMouseMove(const QPoint& pos);
    bool OnLBup(const QPoint& pos);

    void OnPaint(QPainter& painter) const;

private:
    Shape* pcshape_[N];
    int count_;
    Shape* rubber_;
    QPoint start_;
    int kind_;
};
