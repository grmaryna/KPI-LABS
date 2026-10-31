#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

#include "canvas.h"

namespace {

class MainWindow : public QMainWindow {
public:
    MainWindow() {
        setWindowTitle("Lab2");
        resize(800, 600);

        canvas_ = new Canvas(this);
        setCentralWidget(canvas_);

        menuBar()->setNativeMenuBar(false);

        QMenu* fileMenu = menuBar()->addMenu("Файл");
        QAction* exitAct = fileMenu->addAction("Вихід");
        connect(exitAct, &QAction::triggered, this, [this] { close(); });

        objectsMenu_ = menuBar()->addMenu("Об'єкти");
        pointAct_   = AddShapeAction("Крапка", ST_POINT);
        lineAct_    = AddShapeAction("Лінія", ST_LINE);
        rectAct_    = AddShapeAction("Прямокутник", ST_RECT);
        ellipseAct_ = AddShapeAction("Еліпс", ST_ELLIPSE);
        connect(objectsMenu_, &QMenu::aboutToShow, this, [this] { OnInitMenuPopup(); });

        QMenu* helpMenu = menuBar()->addMenu("Довідка");
        QAction* aboutAct = helpMenu->addAction("Про програму");
        connect(aboutAct, &QAction::triggered, this, [this] {
            QMessageBox::information(this, "Про програму",
                                     "Lab2 — графічний редактор об'єктів.");
        });

        OnInitMenuPopup();
    }

private:
    QAction* AddShapeAction(const QString& title, ShapeType type) {
        QAction* act = objectsMenu_->addAction(title);
        act->setCheckable(true);
        connect(act, &QAction::triggered, this, [this, type] {
            canvas_->SetType(type);
            OnInitMenuPopup();
        });
        return act;
    }

    void OnInitMenuPopup() {
        const ShapeType t = canvas_->Type();
        pointAct_->setChecked(t == ST_POINT);
        lineAct_->setChecked(t == ST_LINE);
        rectAct_->setChecked(t == ST_RECT);
        ellipseAct_->setChecked(t == ST_ELLIPSE);
    }

    Canvas* canvas_ = nullptr;
    QMenu* objectsMenu_ = nullptr;
    QAction* pointAct_ = nullptr;
    QAction* lineAct_ = nullptr;
    QAction* rectAct_ = nullptr;
    QAction* ellipseAct_ = nullptr;
};

}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}