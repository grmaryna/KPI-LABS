#include "shape_registry.h"

#include <QPainter>
#include <QPixmap>
#include <algorithm>
#include <cstring>
#include <memory>
#include <vector>

namespace {

std::vector<ShapeInfo>& Storage() {
    static std::vector<ShapeInfo> shapes;
    return shapes;
}

const std::vector<ShapeInfo>& Sorted() {
    static bool sorted = false;
    std::vector<ShapeInfo>& v = Storage();
    if (!sorted) {
        std::stable_sort(v.begin(), v.end(),
                         [](const ShapeInfo& a, const ShapeInfo& b) { return a.order < b.order; });
        sorted = true;
    }
    return v;
}

}

ShapeRegistrar::ShapeRegistrar(const ShapeInfo& info) {
    Storage().push_back(info);
}

int ShapeKindCount() {
    return static_cast<int>(Sorted().size());
}

const ShapeInfo& GetShapeInfo(int kind) {
    return Sorted()[kind];
}

const ShapeInfo* FindShapeInfo(const char* id) {
    for (const ShapeInfo& info : Sorted())
        if (std::strcmp(info.id, id) == 0)
            return &info;
    return nullptr;
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