#pragma once

#include <QDialog>
#include <QString>
#include <functional>
#include <vector>

class QPushButton;
class QTableWidget;

class MyTable : public QDialog {
public:
    struct Row {
        QString type;
        int x1, y1, x2, y2;
    };

    using RowCallback = std::function<void(int row)>;
    using Callback = std::function<void()>;

    explicit MyTable(QWidget* parent = nullptr);

    void SetRows(const std::vector<Row>& rows);
    void SelectRow(int row);
    int CurrentRow() const;

    void SetOnRowSelected(RowCallback callback);
    void SetOnRowDeleteRequested(RowCallback callback);
    void SetOnClosed(Callback callback);

protected:
    void done(int result) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void UpdateDeleteButton();
    void RequestDelete();

    QTableWidget* table_;
    QPushButton* deleteButton_;
    bool updating_ = false;
    RowCallback onRowSelected_;
    RowCallback onRowDeleteRequested_;
    Callback onClosed_;
};