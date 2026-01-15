#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QDateEdit>
#include <QCheckBox>
#include <QComboBox>

#include <vector>
#include <utility>

#include "ContactModel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(const std::string& file, QWidget* parent = nullptr);

private:
    ContactModel model;

    QTableWidget* table = nullptr;

    QPushButton *addBtn = nullptr, *editBtn = nullptr, *delBtn = nullptr;
    QPushButton *ascBtn = nullptr, *descBtn = nullptr;

    QLineEdit* searchEdit = nullptr;

    // выбор столбца сортировки
    QComboBox* columnBox = nullptr;

    QComboBox* storageBox = nullptr;

    // диапазон дат
    QCheckBox* cbUseDateFilter = nullptr;
    QDateEdit* dateFrom = nullptr;
    QDateEdit* dateTo = nullptr;

    // sorting state
    int currentSortColumn = 1;                  // 1=Last name (0 — скрытый Idx)
    Qt::SortOrder currentSortOrder = Qt::AscendingOrder;

private:
    void buildUi();

    void reloadTable();                         // загрузка с учетом поиска/диапазона даты
    void sortByColumn(int col, Qt::SortOrder order); // пустые -> в конец

    bool matchesSearchAllFields(const Contact& c, const QString& q) const;
    bool matchesDateRange(const Contact& c) const;

    void fillRow(int row, int modelIndex, const Contact& c);
    int selectedModelIndex() const;
};
