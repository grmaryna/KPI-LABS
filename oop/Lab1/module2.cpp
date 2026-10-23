#include "module2.h"
#include "utils.h"
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>

namespace {
QStringList FacultyGroups() {
    return {"ІП-51", "ІП-52", "ІП-53", "ІП-54", "ІП-55",
            "ІС-51", "ІС-52", "ІС-53",
            "ІА-51", "ІА-52", "ІА-53"};
}
}

bool ShowGroupDialog(QWidget* parent, QString& chosen) {
    QDialog dlg(parent);
    dlg.setWindowTitle("Робота2 — вибір групи");
    dlg.setMinimumSize(320, 320);

    auto* hint   = new QLabel("Оберіть групу зі списку:", &dlg);
    auto* list   = new QListWidget(&dlg);
    auto* yes    = new QPushButton("Так", &dlg);
    auto* cancel = new QPushButton("Відміна", &dlg);

    list->addItems(FacultyGroups());
    yes->setEnabled(false);
    yes->setDefault(true);

    auto* buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(yes);
    buttons->addWidget(cancel);

    auto* root = new QVBoxLayout(&dlg);
    root->addWidget(hint);
    root->addWidget(list);
    root->addLayout(buttons);

    QObject::connect(list, &QListWidget::itemSelectionChanged, &dlg,
                     [list, yes] { yes->setEnabled(list->currentItem() != nullptr); });
    QObject::connect(list, &QListWidget::itemDoubleClicked, &dlg, [&dlg] { dlg.done(DR_Yes); });
    QObject::connect(yes,    &QPushButton::clicked, &dlg, [&dlg] { dlg.done(DR_Yes); });
    QObject::connect(cancel, &QPushButton::clicked, &dlg, [&dlg] { dlg.done(DR_Cancel); });

    if (dlg.exec() == DR_Yes && list->currentItem() != nullptr) {
        chosen = list->currentItem()->text();
        return true;
    }
    return false;
}