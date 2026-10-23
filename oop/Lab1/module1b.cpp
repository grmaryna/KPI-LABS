#include "module1b.h"
#include "utils.h"
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

int ShowSecondDialog(QWidget* parent) {
    QDialog dlg(parent);
    dlg.setWindowTitle("Робота1 — вікно 2 із 2");
    dlg.setMinimumWidth(340);

    auto* text   = new QLabel("Це друге вікно діалогу.\n«< Назад» поверне до першого вікна.", &dlg);
    auto* back   = new QPushButton("< Назад", &dlg);
    auto* yes    = new QPushButton("Так", &dlg);
    auto* cancel = new QPushButton("Відміна", &dlg);
    yes->setDefault(true);

    auto* buttons = new QHBoxLayout;
    buttons->addWidget(back);
    buttons->addStretch();
    buttons->addWidget(yes);
    buttons->addWidget(cancel);

    auto* root = new QVBoxLayout(&dlg);
    root->addWidget(text);
    root->addLayout(buttons);

    QObject::connect(back,   &QPushButton::clicked, &dlg, [&dlg] { dlg.done(DR_Back); });
    QObject::connect(yes,    &QPushButton::clicked, &dlg, [&dlg] { dlg.done(DR_Yes); });
    QObject::connect(cancel, &QPushButton::clicked, &dlg, [&dlg] { dlg.done(DR_Cancel); });

    return dlg.exec();
}