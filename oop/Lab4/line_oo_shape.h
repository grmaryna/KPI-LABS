#pragma once

#include "ellipse_shape.h"
#include "line_shape.h"

// Лінія з кружечками на кінцях — множинне успадкування:
//   лінія      -> LineShape::Show
//   кружечки   -> EllipseShape::Show
// Ввід: по двох точках (початок і кінець лінії).
class LineOOShape : public LineShape, public EllipseShape {
public:
    // Обидва базові класи перевизначають Show/Trace, тому остаточний
    // перевизначувач для складеної фігури має бути єдиним — оголошуємо його тут.
    void Show(QPainter& painter) const override;
    void Trace(QPainter& painter) const override;

private:
    static constexpr int kRadius = 6;  // радіус кружечка, пікселі
};
