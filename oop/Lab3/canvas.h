#pragma once
#include <QIcon>
#include <QWidget>

#include "shape.h"

enum ShapeType {
    ST_POINT,
    ST_LINE,
    ST_RECT,
    ST_ELLIPSE
};

class Canvas : public QWidget {
public:
    static constexpr int N = 107;

    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas() override;

    Canvas(const Canvas&) = delete;
    Canvas& operator=(const Canvas&) = delete;

    void SetType(ShapeType type);
    ShapeType Type() const;

    static QIcon MakeIcon(ShapeType type);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    Shape* CreateShape(ShapeType type) const;

    Shape* pcshape_[N];
    int count_;
    Shape* rubber_;
    QPoint start_;
    ShapeType type_;
};