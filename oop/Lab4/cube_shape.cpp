#include "cube_shape.h"

#include <QPainter>
#include <algorithm>

#include "shape_parts.h"

namespace {
template <class Op>
void ForRect(int x, int y, int w, int h, Op op) {
    op(x + w / 2, y + h / 2, x + w, y + h);
}
}

CubeShape::Faces CubeShape::Layout() const {
    const int l = std::min(x1_, x2_);
    const int r = std::max(x1_, x2_);
    const int t = std::min(y1_, y2_);
    const int b = std::max(y1_, y2_);
    const int depth = std::min(r - l, b - t) / 3;

    Faces f;
    f.w = ((r - l) - depth) / 2 * 2;
    f.h = ((b - t) - depth) / 2 * 2;
    f.fx = l;
    f.fy = b - f.h;
    f.bx = r - f.w;
    f.by = t;
    return f;
}

void CubeShape::Show(QPainter& painter) const {
    const Faces f = Layout();
    const auto wire = [](RectShape& rect) { rect.SetFilled(false); };

    ForRect(f.bx, f.by, f.w, f.h, [&](int cx, int cy, int px, int py) {
        ShowPart<RectShape>(*this, painter, cx, cy, px, py, wire);
    });

    for (int i = 0; i < 4; ++i) {
        const int dx = (i % 2) * f.w;
        const int dy = (i / 2) * f.h;
        ShowPart<LineShape>(*this, painter,
                            f.fx + dx, f.fy + dy, f.bx + dx, f.by + dy);
    }

    ForRect(f.fx, f.fy, f.w, f.h, [&](int cx, int cy, int px, int py) {
        ShowPart<RectShape>(*this, painter, cx, cy, px, py, wire);
    });
}

void CubeShape::Trace(QPainter& painter) const {
    const Faces f = Layout();

    ForRect(f.bx, f.by, f.w, f.h, [&](int cx, int cy, int px, int py) {
        TracePart<RectShape>(*this, painter, cx, cy, px, py);
    });
    for (int i = 0; i < 4; ++i) {
        const int dx = (i % 2) * f.w;
        const int dy = (i / 2) * f.h;
        TracePart<LineShape>(*this, painter,
                             f.fx + dx, f.fy + dy, f.bx + dx, f.by + dy);
    }
    ForRect(f.fx, f.fy, f.w, f.h, [&](int cx, int cy, int px, int py) {
        TracePart<RectShape>(*this, painter, cx, cy, px, py);
    });
}
