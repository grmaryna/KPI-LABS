#pragma once

#include <QPoint>
#include <functional>

#include "shape.h"

class QPainter;

class MyEditor {
public:
    static constexpr int N = 107;

    static MyEditor* Instance();
    static void DestroyInstance();

    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;

    void SetShapeKind(int kind);
    int ShapeKind() const;

    int Count() const;
    const Shape& ShapeAt(int index) const;
    bool AddShape(Shape* shape);
    bool Remove(int index);
    void Clear();

    int Selected() const;
    void Select(int index);

    bool OnLBdown(const QPoint& pos);
    bool OnMouseMove(const QPoint& pos);
    bool OnLBup(const QPoint& pos);

    void OnPaint(QPainter& painter) const;

    void SetOnShapesChanged(std::function<void()> callback);

private:
    MyEditor();
    ~MyEditor();

    void NotifyChanged();

    static MyEditor* instance_;

    Shape* pcshape_[N];
    int count_;
    Shape* rubber_;
    QPoint start_;
    int kind_;
    int selected_;
    std::function<void()> onShapesChanged_;
};