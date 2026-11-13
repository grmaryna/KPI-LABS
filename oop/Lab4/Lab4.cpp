#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include <vector>

#include "canvas.h"
#include "my_editor.h"
#include "shape_registry.h"

namespace {

class MainWindow : public QMainWindow {
public:
    MainWindow() : editor_(new MyEditor) {
        resize(800, 600);

        canvas_ = new Canvas(editor_, this);
        setCentralWidget(canvas_);

        menuBar()->setNativeMenuBar(false);

        QMenu* fileMenu = menuBar()->addMenu("Файл");
        QAction* exitAct = fileMenu->addAction("Вихід");
        connect(exitAct, &QAction::triggered, this, [this] { OnExit(); });

        objectsMenu_ = menuBar()->addMenu("Об'єкти");
        toolBar_ = addToolBar("Об'єкти");
        toolBar_->setMovable(false);
        toolBar_->setIconSize(QSize(32, 32));

        group_ = new QActionGroup(this);
        for (int kind = 0; kind < ShapeKindCount(); ++kind)
            actions_.push_back(AddShapeAction(kind));

        QMenu* helpMenu = menuBar()->addMenu("Довідка");
        QAction* aboutAct = helpMenu->addAction("Про програму");
        connect(aboutAct, &QAction::triggered, this, [this] { OnAbout(); });

        SyncState();
    }

    ~MainWindow() override {
        canvas_->SetEditor(nullptr);
        delete editor_;
        editor_ = nullptr;
    }

    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

private:
    QAction* AddShapeAction(int kind) {
        const ShapeInfo& info = GetShapeInfo(kind);
        const QString title = QString::fromUtf8(info.title);

        QAction* act = new QAction(MakeShapeIcon(kind), title, this);
        act->setCheckable(true);
        act->setToolTip(QString::fromUtf8(info.tooltip));
        act->setIconVisibleInMenu(false);
        group_->addAction(act);
        objectsMenu_->addAction(act);
        toolBar_->addAction(act);
        connect(act, &QAction::triggered, this, [this, kind] { OnSelectShape(kind); });
        return act;
    }

    void OnSelectShape(int kind) {
        editor_->SetShapeKind(kind);
        SyncState();
    }

    void OnExit() { close(); }

    void OnAbout() {
        QMessageBox::information(this, "Про програму",
                                 "Lab4 — графічний редактор об'єктів.");
    }

    void SyncState() {
        const int kind = editor_->ShapeKind();
        for (int i = 0; i < static_cast<int>(actions_.size()); ++i)
            actions_[i]->setChecked(i == kind);
        setWindowTitle("Lab4 — " + QString::fromUtf8(GetShapeInfo(kind).title));
    }

    MyEditor* editor_ = nullptr;
    Canvas* canvas_ = nullptr;
    QMenu* objectsMenu_ = nullptr;
    QToolBar* toolBar_ = nullptr;
    QActionGroup* group_ = nullptr;
    std::vector<QAction*> actions_;
};

}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
