#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QStandardPaths>
#include <QStatusBar>
#include <QToolBar>
#include <vector>

#include "canvas.h"
#include "my_editor.h"
#include "my_table.h"
#include "shape_file.h"
#include "shape_registry.h"

namespace {

class MainWindow : public QMainWindow {
public:
    MainWindow() {
        resize(800, 600);

        canvas_ = new Canvas(this);
        setCentralWidget(canvas_);

        menuBar()->setNativeMenuBar(false);

        QMenu* fileMenu = menuBar()->addMenu("Файл");
        QAction* openAct = fileMenu->addAction("Відкрити...");
        openAct->setShortcut(QKeySequence::Open);
        connect(openAct, &QAction::triggered, this, [this] { OnOpen(); });
        QAction* saveAct = fileMenu->addAction("Зберегти як...");
        saveAct->setShortcut(QKeySequence::Save);
        connect(saveAct, &QAction::triggered, this, [this] { OnSave(); });
        fileMenu->addSeparator();
        QAction* exitAct = fileMenu->addAction("Вихід");
        connect(exitAct, &QAction::triggered, this, [this] { close(); });

        objectsMenu_ = menuBar()->addMenu("Об'єкти");
        toolBar_ = addToolBar("Об'єкти");
        toolBar_->setMovable(false);
        toolBar_->setIconSize(QSize(32, 32));

        group_ = new QActionGroup(this);
        for (int kind = 0; kind < ShapeKindCount(); ++kind)
            actions_.push_back(AddShapeAction(kind));

        QMenu* viewMenu = menuBar()->addMenu("Вигляд");
        tableAct_ = viewMenu->addAction("Таблиця об'єктів");
        tableAct_->setCheckable(true);
        tableAct_->setShortcut(QKeySequence("Ctrl+T"));
        connect(tableAct_, &QAction::triggered, this, [this](bool on) { OnToggleTable(on); });

        QMenu* helpMenu = menuBar()->addMenu("Довідка");
        QAction* aboutAct = helpMenu->addAction("Про програму");
        connect(aboutAct, &QAction::triggered, this, [this] { OnAbout(); });

        table_ = new MyTable(this);
        table_->SetOnRowSelected([this](int row) { OnTableRowSelected(row); });
        table_->SetOnRowDeleteRequested([this](int row) { OnTableRowDelete(row); });
        table_->SetOnClosed([this] { OnTableClosed(); });

        MyEditor::Instance()->SetOnShapesChanged([this] { RefreshTable(); });

        SyncState();
    }

    ~MainWindow() override {
        MyEditor::Instance()->SetOnShapesChanged(nullptr);
    }

    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

protected:
    void closeEvent(QCloseEvent* event) override {
        table_->SetOnClosed(nullptr);
        table_->close();
        QMainWindow::closeEvent(event);
    }

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
        MyEditor::Instance()->SetShapeKind(kind);
        SyncState();
    }

    void OnAbout() {
        QMessageBox::information(this, "Про програму",
                                 "Lab5 — графічний редактор об'єктів "
                                 "(багатовіконний інтерфейс).");
    }

    void SyncState() {
        const int kind = MyEditor::Instance()->ShapeKind();
        for (int i = 0; i < static_cast<int>(actions_.size()); ++i)
            actions_[i]->setChecked(i == kind);
        setWindowTitle("Lab5 — " + QString::fromUtf8(GetShapeInfo(kind).title));
    }

    void OnSave() {
        const QString path = AskFile(QFileDialog::AcceptSave, "Зберегти множину об'єктів",
                                     DefaultDir() + "/shapes.txt");
        if (path.isEmpty())
            return;
        QString error;
        if (!SaveShapes(*MyEditor::Instance(), path, &error))
            QMessageBox::warning(this, "Запис файлу", error);
        else
            statusBar()->showMessage("Збережено: " + path, 5000);
    }

    void OnOpen() {
        const QString path = AskFile(QFileDialog::AcceptOpen, "Відкрити множину об'єктів",
                                     DefaultDir());
        if (path.isEmpty())
            return;
        QString error;
        if (!LoadShapes(*MyEditor::Instance(), path, &error))
            QMessageBox::warning(this, "Читання файлу", error);
        else
            statusBar()->showMessage(
                QString("Завантажено об'єктів: %1").arg(MyEditor::Instance()->Count()), 5000);
        canvas_->update();
    }

    QString AskFile(QFileDialog::AcceptMode mode, const QString& title, const QString& start) {
        QFileDialog dlg(this, title, start, "Текстові файли (*.txt);;Усі файли (*)");
        dlg.setOption(QFileDialog::DontUseNativeDialog, true);
        dlg.setAcceptMode(mode);
        if (mode == QFileDialog::AcceptSave) {
            dlg.setFileMode(QFileDialog::AnyFile);
            dlg.setDefaultSuffix("txt");
        } else {
            dlg.setFileMode(QFileDialog::ExistingFile);
        }
        if (dlg.exec() != QDialog::Accepted || dlg.selectedFiles().isEmpty())
            return QString();
        return dlg.selectedFiles().first();
    }

    static QString DefaultDir() {
        const QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        return dir.isEmpty() ? QDir::homePath() : dir;
    }

    void OnToggleTable(bool on) {
        if (on) {
            PlaceTableBesideWindow();
            table_->show();
            table_->raise();
            RefreshTable();
        } else {
            MyEditor::Instance()->Select(-1);
            table_->hide();
            canvas_->update();
        }
    }

    void OnTableClosed() {
        tableAct_->setChecked(false);
        MyEditor::Instance()->Select(-1);
        canvas_->update();
    }

    void OnTableRowSelected(int row) {
        MyEditor::Instance()->Select(row);
        canvas_->update();
    }

    void OnTableRowDelete(int row) {
        MyEditor::Instance()->Remove(row);
        canvas_->update();
    }

    void RefreshTable() {
        canvas_->update();
        if (!table_->isVisible())
            return;
        const MyEditor* editor = MyEditor::Instance();
        std::vector<MyTable::Row> rows;
        for (int i = 0; i < editor->Count(); ++i) {
            const Shape& s = editor->ShapeAt(i);
            const ShapeInfo* info = FindShapeInfo(s.Id());
            rows.push_back({QString::fromUtf8(info ? info->title : s.Id()),
                            s.X1(), s.Y1(), s.X2(), s.Y2()});
        }
        table_->SetRows(rows);
        table_->SelectRow(editor->Selected());
    }

    void PlaceTableBesideWindow() {
        const QRect avail = screen()->availableGeometry();
        QPoint p = frameGeometry().topRight() + QPoint(8, 0);
        if (p.x() + table_->width() > avail.right())
            p = frameGeometry().topLeft() + QPoint(80, 80);
        table_->move(p);
    }

    Canvas* canvas_ = nullptr;
    MyTable* table_ = nullptr;
    QAction* tableAct_ = nullptr;
    QMenu* objectsMenu_ = nullptr;
    QToolBar* toolBar_ = nullptr;
    QActionGroup* group_ = nullptr;
    std::vector<QAction*> actions_;
};

}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    int result = 0;
    {
        MainWindow w;
        w.show();
        result = app.exec();
    }
    MyEditor::DestroyInstance();
    return result;
}