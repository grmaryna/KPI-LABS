#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include "module1.h"
#include "module1b.h"
#include "module2.h"
#include "utils.h"

namespace {

class MainWindow : public QMainWindow {
public:
    MainWindow() {
        setWindowTitle("Lab1");
        resize(520, 260);

        label_ = new QLabel("Оберіть пункт меню «Робота1» або «Робота2»", this);
        label_->setAlignment(Qt::AlignCenter);
        label_->setWordWrap(true);
        QFont f = label_->font();
        f.setPointSize(16);
        label_->setFont(f);
        setCentralWidget(label_);

        menuBar()->setNativeMenuBar(false);
        QAction* work1 = menuBar()->addAction("Робота1");
        QAction* work2 = menuBar()->addAction("Робота2");
        connect(work1, &QAction::triggered, this, [this] { runWork1(); });
        connect(work2, &QAction::triggered, this, [this] { runWork2(); });
    }

private:
    void runWork1() {
        int step = 1;
        int result = DR_Cancel;
        while (true) {
            if (step == 1) {
                result = ShowFirstDialog(this);
                if (result == DR_Next) { step = 2; continue; }
            } else {
                result = ShowSecondDialog(this);
                if (result == DR_Back) { step = 1; continue; }
            }
            break;
        }
        label_->setText(result == DR_Yes ? "Робота1: у другому вікні натиснуто «Так»"
                                         : "Робота1: скасовано");
    }

    void runWork2() {
        QString chosen;
        if (ShowGroupDialog(this, chosen))
            label_->setText("Вибрана група: " + chosen);
    }

    QLabel* label_ = nullptr;
};

}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}