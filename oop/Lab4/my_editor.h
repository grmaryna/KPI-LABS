#pragma once

#include <QPoint>

#include "shape.h"

class QPainter;

// Редактор графічних об'єктів: зберігає масив фігур, поточний тип фігури та
// «гумовий» слід. Не залежить від віджетів — отримує лише координати подій і
// QPainter, тому легко розширюється новими типами фігур.
//
// У Lab4 (парний номер у списку) екземпляр MyEditor створюється ДИНАМІЧНО
// (new) у головному вікні й знищується (delete) в його деструкторі.
class MyEditor {
public:
    static constexpr int N = 107;  // Ж + 100, Ж = 7

    MyEditor();
    ~MyEditor();
    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;

    void SetShapeKind(int kind);
    int ShapeKind() const;
    int Count() const;

    // Повертають true, якщо зображення треба перемалювати.
    bool OnLBdown(const QPoint& pos);
    bool OnMouseMove(const QPoint& pos);
    bool OnLBup(const QPoint& pos);

    void OnPaint(QPainter& painter) const;

private:
    Shape* pcshape_[N];  // статичний масив вказівників на Shape
    int count_;
    Shape* rubber_;      // фігура, що зараз вводиться («гумовий» слід)
    QPoint start_;
    int kind_;
};
