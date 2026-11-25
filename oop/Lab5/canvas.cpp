#include "canvas.h"

#include <QMouseEvent>
#include <QPainter>

#include "my_editor.h"

Canvas::Canvas(QWidget* parent) : QWidget(parent) {
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void Canvas::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    MyEditor::Instance()->OnPaint(painter);
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton &&
        MyEditor::Instance()->OnLBdown(event->position().toPoint()))
        update();
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (MyEditor::Instance()->OnMouseMove(event->position().toPoint()))
        update();
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton &&
        MyEditor::Instance()->OnLBup(event->position().toPoint()))
        update();
}