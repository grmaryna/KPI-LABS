#pragma once

#include "shape.h"

class QPainter;

template <class Part, class Whole, class Config>
void ShowPart(const Whole& whole, QPainter& painter,
              int x1, int y1, int x2, int y2, Config config) {
    Part part(whole);
    part.Set(x1, y1, x2, y2);
    config(part);
    part.Part::Show(painter);
}

template <class Part, class Whole>
void ShowPart(const Whole& whole, QPainter& painter,
              int x1, int y1, int x2, int y2) {
    ShowPart<Part>(whole, painter, x1, y1, x2, y2, [](Part&) {});
}

template <class Part, class Whole>
void TracePart(const Whole& whole, QPainter& painter,
               int x1, int y1, int x2, int y2) {
    Part part(whole);
    part.Set(x1, y1, x2, y2);
    part.Part::Trace(painter);
}
