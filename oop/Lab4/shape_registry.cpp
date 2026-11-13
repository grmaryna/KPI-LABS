#include "shape_registry.h"

#include <QPainter>
#include <QPixmap>
#include <memory>

#include "cube_shape.h"
#include "ellipse_shape.h"
#include "line_oo_shape.h"
#include "line_shape.h"
#include "point_shape.h"
#include "rect_shape.h"

namespace {

template <class T>
Shape* Make() {
    return new T;
}

const ShapeInfo kShapes[] = {
    {"Крапка", "Крапка", &Make<PointShape>, {16, 16, 16, 16}},
    {"Лінія", "Лінія", &Make<LineShape>, {4, 28, 28, 4}},
    {"Прямокутник", "Прямокутник (від центру до кута)", &Make<RectShape>, {16, 16, 27, 22}},
    {"Еліпс", "Еліпс (по протилежних кутах)", &Make<EllipseShape>, {4, 8, 28, 24}},
    {"Лінія з кружечками", "Лінія з кружечками", &Make<LineOOShape>, {8, 24, 24, 8}},
    {"Каркас куба", "Каркас куба", &Make<CubeShape>, {4, 5, 28, 29}},
};

}

int ShapeKindCount() {
    return static_cast<int>(sizeof(kShapes) / sizeof(kShapes[0]));
}

const ShapeInfo& GetShapeInfo(int kind) {
    return kShapes[kind];
}

QIcon MakeShapeIcon(int kind) {
    const ShapeInfo& info = GetShapeInfo(kind);

    QPixmap pm(32, 32);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    std::unique_ptr<Shape> shape(info.create());
    shape->Set(info.icon[0], info.icon[1], info.icon[2], info.icon[3]);
    shape->Show(p);
    p.end();
    return QIcon(pm);
}
