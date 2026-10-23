#include "module1.h"
#include "utils.h"
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

int ShowFirstDialog(QWidget* parent) {
    QDialog dlg(parent);
    dlg.setWindowTitle("Робота1 — вікно 1 із 2");
    dlg.setMinimumWidth(340);

    auto* text   = new QLabel("Це перше вікно діалогу.\nНатисніть «Далі >», щоб перейти до другого.", &dlg);
    auto* next   = new QPushButton("Далі >", &dlg);
    auto* cancel = new QPushButton("Відміна", &dlg);
    next->setDefault(true);

    auto* buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(next);
    buttons->addWidget(cancel);

    auto* root = new QVBoxLayout(&dlg);
    root->addWidget(text);
    root->addLayout(buttons);

    QObject::connect(next,   &QPushButton::clicked, &dlg, [&dlg] { dlg.done(DR_Next); });
    QObject::connect(cancel, &QPushButton::clicked, &dlg, [&dlg] { dlg.done(DR_Cancel); });

    return dlg.exec();
}