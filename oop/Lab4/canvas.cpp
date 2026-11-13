#include "canvas.h"

#include <QMouseEvent>
#include <QPainter>

#include "my_editor.h"

Canvas::Canvas(MyEditor* editor, QWidget* parent)
    : QWidget(parent), editor_(editor) {
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void Canvas::SetEditor(MyEditor* editor) {
    editor_ = editor;
}

void Canvas::paintEvent(QPaintEvent*) {
    if (editor_ == nullptr)
        return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    editor_->OnPaint(painter);
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (editor_ != nullptr && event->button() == Qt::LeftButton &&
        editor_->OnLBdown(event->position().toPoint()))
        update();
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (editor_ != nullptr && editor_->OnMouseMove(event->position().toPoint()))
        update();
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (editor_ != nullptr && event->button() == Qt::LeftButton &&
        editor_->OnLBup(event->position().toPoint()))
        update();
}
