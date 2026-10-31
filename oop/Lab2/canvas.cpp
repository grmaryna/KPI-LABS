#include "canvas.h"

#include <QMouseEvent>
#include <QPainter>

#include "ellipse_shape.h"
#include "line_shape.h"
#include "point_shape.h"
#include "rect_shape.h"

Canvas::Canvas(QWidget* parent)
    : QWidget(parent),
      pcshape_(new Shape*[N]),
      count_(0),
      rubber_(nullptr),
      type_(ST_POINT) {
    for (int i = 0; i < N; ++i)
        pcshape_[i] = nullptr;
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

Canvas::~Canvas() {
    for (int i = 0; i < count_; ++i)
        delete pcshape_[i];
    delete[] pcshape_;
    delete rubber_;
}

void Canvas::SetType(ShapeType type) {
    type_ = type;
}

ShapeType Canvas::Type() const {
    return type_;
}

Shape* Canvas::CreateShape(ShapeType type) const {
    switch (type) {
        case ST_POINT:   return new PointShape;
        case ST_LINE:    return new LineShape;
        case ST_RECT:    return new RectShape;
        case ST_ELLIPSE: return new EllipseShape;
    }
    return nullptr;
}

void Canvas::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    for (int i = 0; i < count_; ++i)
        pcshape_[i]->Show(painter);

    if (rubber_ != nullptr) {
        painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
        painter.setBrush(Qt::NoBrush);
        rubber_->Trace(painter);
    }
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton || count_ >= N)
        return;
    start_ = event->position().toPoint();
    delete rubber_;
    rubber_ = CreateShape(type_);
    rubber_->Set(start_.x(), start_.y(), start_.x(), start_.y());
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (rubber_ == nullptr)
        return;
    const QPoint p = event->position().toPoint();
    rubber_->Set(start_.x(), start_.y(), p.x(), p.y());
    update();
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (rubber_ == nullptr || event->button() != Qt::LeftButton)
        return;
    const QPoint p = event->position().toPoint();
    rubber_->Set(start_.x(), start_.y(), p.x(), p.y());
    pcshape_[count_++] = rubber_;
    rubber_ = nullptr;
    update();
}