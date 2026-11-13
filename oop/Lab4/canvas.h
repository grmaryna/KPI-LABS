#pragma once

#include <QWidget>

class MyEditor;

// Віджет-полотно: лише передає події миші та перемальовування редактору.
// Редактором Canvas не володіє (його створює й знищує головне вікно).
class Canvas : public QWidget {
public:
    explicit Canvas(MyEditor* editor, QWidget* parent = nullptr);

    void SetEditor(MyEditor* editor);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    MyEditor* editor_;
};
