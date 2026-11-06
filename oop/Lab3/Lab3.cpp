#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>

#include "canvas.h"

namespace {

class MainWindow : public QMainWindow {
public:
    MainWindow() {
        resize(800, 600);

        canvas_ = new Canvas(this);
        setCentralWidget(canvas_);

        menuBar()->setNativeMenuBar(false);

        // Порядок меню: Файл — Об'єкти — Довідка
        QMenu* fileMenu = menuBar()->addMenu("Файл");
        QAction* exitAct = fileMenu->addAction("Вихід");
        connect(exitAct, &QAction::triggered, this, [this] { OnExit(); });

        objectsMenu_ = menuBar()->addMenu("Об'єкти");
        toolBar_ = addToolBar("Об'єкти");
        toolBar_->setMovable(false);
        toolBar_->setIconSize(QSize(32, 32));

        group_ = new QActionGroup(this);
        pointAct_   = AddShapeAction("Крапка", ST_POINT);
        lineAct_    = AddShapeAction("Лінія", ST_LINE);
        rectAct_    = AddShapeAction("Прямокутник", ST_RECT);
        ellipseAct_ = AddShapeAction("Еліпс", ST_ELLIPSE);

        QMenu* helpMenu = menuBar()->addMenu("Довідка");
        QAction* aboutAct = helpMenu->addAction("Про програму");
        connect(aboutAct, &QAction::triggered, this, [this] { OnAbout(); });

        SyncState();
    }

private:
    // Одна й та сама дія додається і в меню, і в Toolbar,
    // тому кнопки та пункти меню завжди відповідають одне одному.
    QAction* AddShapeAction(const QString& title, ShapeType type) {
        QAction* act = new QAction(Canvas::MakeIcon(type), title, this);
        act->setCheckable(true);
        act->setToolTip(title);                 // підказка (tooltip)
        act->setIconVisibleInMenu(false);
        group_->addAction(act);
        objectsMenu_->addAction(act);
        toolBar_->addAction(act);
        connect(act, &QAction::triggered, this, [this, type] { OnSelectShape(type); });
        return act;
    }

    // ---- обробники (методи класу) ----
    void OnSelectShape(ShapeType type) {
        canvas_->SetType(type);
        SyncState();
    }

    void OnExit() { close(); }

    void OnAbout() {
        QMessageBox::information(this, "Про програму",
                                 "Lab3 — графічний редактор об'єктів.");
    }

    // Позначка поточного типу: у заголовку вікна (Ж mod 2 = 1)
    void SyncState() {
        const ShapeType t = canvas_->Type();
        pointAct_->setChecked(t == ST_POINT);
        lineAct_->setChecked(t == ST_LINE);
        rectAct_->setChecked(t == ST_RECT);
        ellipseAct_->setChecked(t == ST_ELLIPSE);

        QString name;
        switch (t) {
            case ST_POINT:   name = "Крапка"; break;
            case ST_LINE:    name = "Лінія"; break;
            case ST_RECT:    name = "Прямокутник"; break;
            case ST_ELLIPSE: name = "Еліпс"; break;
        }
        setWindowTitle("Lab3 — " + name);
    }

    Canvas* canvas_ = nullptr;
    QMenu* objectsMenu_ = nullptr;
    QToolBar* toolBar_ = nullptr;
    QActionGroup* group_ = nullptr;
    QAction* pointAct_ = nullptr;
    QAction* lineAct_ = nullptr;
    QAction* rectAct_ = nullptr;
    QAction* ellipseAct_ = nullptr;
};

}  // namespace

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}