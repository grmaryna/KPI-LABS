#include "my_table.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

MyTable::MyTable(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Таблиця об'єктів");
    setModal(false);
    resize(460, 380);

    table_ = new QTableWidget(0, 6, this);
    table_->setHorizontalHeaderLabels({"№", "Тип", "x1", "y1", "x2", "y2"});
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->verticalHeader()->setVisible(false);
    table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    deleteButton_ = new QPushButton("Вилучити вибраний об'єкт", this);
    deleteButton_->setAutoDefault(false);
    deleteButton_->setEnabled(false);

    QHBoxLayout* buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(deleteButton_);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(table_);
    layout->addLayout(buttons);

    connect(table_, &QTableWidget::itemSelectionChanged, this, [this] {
        UpdateDeleteButton();
        if (!updating_ && onRowSelected_)
            onRowSelected_(CurrentRow());
    });
    connect(deleteButton_, &QPushButton::clicked, this, [this] { RequestDelete(); });
}

void MyTable::SetRows(const std::vector<Row>& rows) {
    updating_ = true;
    table_->clearSelection();
    table_->setRowCount(static_cast<int>(rows.size()));
    for (int i = 0; i < static_cast<int>(rows.size()); ++i) {
        const Row& r = rows[i];
        const QString cells[6] = {QString::number(i + 1), r.type,
                                  QString::number(r.x1), QString::number(r.y1),
                                  QString::number(r.x2), QString::number(r.y2)};
        for (int c = 0; c < 6; ++c) {
            QTableWidgetItem* item = new QTableWidgetItem(cells[c]);
            if (c != 1)
                item->setTextAlignment(Qt::AlignCenter);
            table_->setItem(i, c, item);
        }
    }
    updating_ = false;
    UpdateDeleteButton();
}

void MyTable::SelectRow(int row) {
    updating_ = true;
    if (row >= 0 && row < table_->rowCount()) {
        table_->selectRow(row);
        table_->scrollToItem(table_->item(row, 0));
    } else {
        table_->clearSelection();
    }
    updating_ = false;
    UpdateDeleteButton();
}

int MyTable::CurrentRow() const {
    const QList<QTableWidgetSelectionRange> ranges = table_->selectedRanges();
    return ranges.isEmpty() ? -1 : ranges.first().topRow();
}

void MyTable::SetOnRowSelected(RowCallback callback) {
    onRowSelected_ = std::move(callback);
}

void MyTable::SetOnRowDeleteRequested(RowCallback callback) {
    onRowDeleteRequested_ = std::move(callback);
}

void MyTable::SetOnClosed(Callback callback) {
    onClosed_ = std::move(callback);
}

void MyTable::done(int result) {
    QDialog::done(result);
    if (onClosed_)
        onClosed_();
}

void MyTable::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        RequestDelete();
        return;
    }
    QDialog::keyPressEvent(event);
}

void MyTable::UpdateDeleteButton() {
    deleteButton_->setEnabled(CurrentRow() >= 0);
}

void MyTable::RequestDelete() {
    const int row = CurrentRow();
    if (row >= 0 && onRowDeleteRequested_)
        onRowDeleteRequested_(row);
}