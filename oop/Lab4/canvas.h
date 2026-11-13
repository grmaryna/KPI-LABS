#pragma once

#include <QWidget>

class MyEditor;

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
