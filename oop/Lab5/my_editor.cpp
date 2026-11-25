#include "my_editor.h"

#include <QPainter>

#include "shape_registry.h"

MyEditor* MyEditor::instance_ = nullptr;

MyEditor* MyEditor::Instance() {
    if (instance_ == nullptr)
        instance_ = new MyEditor;
    return instance_;
}

void MyEditor::DestroyInstance() {
    delete instance_;
    instance_ = nullptr;
}

MyEditor::MyEditor() : count_(0), rubber_(nullptr), kind_(0), selected_(-1) {
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

const Shape& MyEditor::ShapeAt(int index) const {
    return *pcshape_[index];
}

bool MyEditor::AddShape(Shape* shape) {
    if (shape == nullptr || count_ >= N)
        return false;
    pcshape_[count_++] = shape;
    NotifyChanged();
    return true;
}

bool MyEditor::Remove(int index) {
    if (index < 0 || index >= count_)
        return false;
    delete pcshape_[index];
    for (int i = index; i + 1 < count_; ++i)
        pcshape_[i] = pcshape_[i + 1];
    pcshape_[--count_] = nullptr;
    selected_ = -1;
    NotifyChanged();
    return true;
}

void MyEditor::Clear() {
    for (int i = 0; i < count_; ++i) {
        delete pcshape_[i];
        pcshape_[i] = nullptr;
    }
    count_ = 0;
    selected_ = -1;
    NotifyChanged();
}

int MyEditor::Selected() const {
    return selected_;
}

void MyEditor::Select(int index) {
    selected_ = (index >= 0 && index < count_) ? index : -1;
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
    Shape* done = rubber_;
    rubber_ = nullptr;
    AddShape(done);
    return true;
}

void MyEditor::OnPaint(QPainter& painter) const {
    for (int i = 0; i < count_; ++i)
        pcshape_[i]->Show(painter);

    if (selected_ >= 0 && selected_ < count_) {
        painter.save();
        painter.setPen(QPen(QColor(0, 120, 215), 2, Qt::DotLine));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(pcshape_[selected_]->Bounds().adjusted(-4, -4, 4, 4));
        painter.restore();
    }

    if (rubber_ != nullptr) {
        painter.save();
        painter.setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        rubber_->Trace(painter);
        painter.restore();
    }
}

void MyEditor::SetOnShapesChanged(std::function<void()> callback) {
    onShapesChanged_ = std::move(callback);
}

void MyEditor::NotifyChanged() {
    if (onShapesChanged_)
        onShapesChanged_();
}