#include "my_editor.h"

#include <QPainter>

#include "shape_registry.h"

MyEditor::MyEditor() : count_(0), rubber_(nullptr), kind_(0) {
    for (int i = 0; i < N; ++i)
        pcshape_[i] = nullptr;
}

MyEditor::~MyEditor() {
    for (int i = 0; i < count_; ++i)
        delete pcshape_[i];
    delete rubber_;
}

void MyEditor::SetShapeKind(int kind) {
    if (kind >= 0 && kind < ShapeKindCount())
        kind_ = kind;
}

int MyEditor::ShapeKind() const {
    return kind_;
}

int MyEditor::Count() const {
    return count_;
}

bool MyEditor::OnLBdown(const QPoint& pos) {
    if (count_ >= N)
        return false;
    start_ = pos;
    delete rubber_;
    rubber_ = GetShapeInfo(kind_).create();
    rubber_->Set(start_.x(), start_.y(), pos.x(), pos.y());
    return true;
}

bool MyEditor::OnMouseMove(const QPoint& pos) {
    if (rubber_ == nullptr)
        return false;
    rubber_->Set(start_.x(), start_.y(), pos.x(), pos.y());
    return true;
}

bool MyEditor::OnLBup(const QPoint& pos) {
    if (rubber_ == nullptr)
        return false;
    rubber_->Set(start_.x(), start_.y(), pos.x(), pos.y());
    pcshape_[count_++] = rubber_;
    rubber_ = nullptr;
    return true;
}

void MyEditor::OnPaint(QPainter& painter) const {
    for (int i = 0; i < count_; ++i)
        pcshape_[i]->Show(painter);

    if (rubber_ != nullptr) {
        painter.save();
        painter.setPen(QPen(Qt::black, 1, Qt::DashLine));  // «гумовий» слід
        painter.setBrush(Qt::NoBrush);
        rubber_->Trace(painter);
        painter.restore();
    }
}
