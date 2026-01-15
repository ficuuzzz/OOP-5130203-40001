#include "MainWindow.h"
#include "ContactDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QRegularExpression>
#include <algorithm>

static QString norm(const std::string& s) {
    return QString::fromStdString(s).trimmed();
}

static QString phonesToString(const Contact& c) {
    QString s;
    const auto v = c.getPhoneNumbers();
    for (int i = 0; i < (int)v.size(); ++i) {
        if (i) s += ", ";
        s += QString::fromStdString(v[i]);
    }
    return s;
}

MainWindow::MainWindow(const std::string& file, QWidget* parent)
    : QMainWindow(parent), model(file)
{
    buildUi();
    reloadTable();
    sortByColumn(currentSortColumn, currentSortOrder);
}

void MainWindow::buildUi() {
    setWindowTitle("PhoneBookQT");
    resize(1100, 650);

    QWidget* w = new QWidget(this);
    auto* root = new QVBoxLayout(w);

    // Row 1: CRUD + Search
    auto* row1 = new QHBoxLayout();

    addBtn = new QPushButton("Add");
    editBtn = new QPushButton("Edit");
    delBtn = new QPushButton("Delete");

    row1->addWidget(addBtn);
    row1->addWidget(editBtn);
    row1->addWidget(delBtn);

    row1->addSpacing(20);
    row1->addWidget(new QLabel("Search:"));

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Search in all fields...");
    row1->addWidget(searchEdit, 1);

    root->addLayout(row1);

    // Row 2: Column + sort buttons + date range
    auto* row2 = new QHBoxLayout();

    row2->addWidget(new QLabel("Column:"));
    columnBox = new QComboBox();
    columnBox->addItems({
        "Last name",
        "First name",
        "Patronymic",
        "Address",
        "Birth date",
        "Email",
        "Phones"
    });
    // columnBox index 0..6 -> table column 1..7
    columnBox->setCurrentIndex(0);
    row2->addWidget(columnBox);

    row2->addSpacing(10);

    ascBtn = new QPushButton("Ascending");
    descBtn = new QPushButton("Descending");
    row2->addWidget(ascBtn);
    row2->addWidget(descBtn);

    row2->addSpacing(20);
    row2->addSpacing(20);
    row2->addWidget(new QLabel("Storage:"));

    storageBox = new QComboBox();
    storageBox->addItem("File");
    storageBox->addItem("PostgreSQL");
    storageBox->setCurrentIndex(0);

    row2->addWidget(storageBox);


    cbUseDateFilter = new QCheckBox("Birth date range");
    dateFrom = new QDateEdit();
    dateTo = new QDateEdit();

    dateFrom->setCalendarPopup(true);
    dateTo->setCalendarPopup(true);
    dateFrom->setDisplayFormat("yyyy-MM-dd");
    dateTo->setDisplayFormat("yyyy-MM-dd");
    dateFrom->setDate(QDate(1900, 1, 1));
    dateTo->setDate(QDate::currentDate());

    row2->addWidget(cbUseDateFilter);
    row2->addWidget(new QLabel("from"));
    row2->addWidget(dateFrom);
    row2->addWidget(new QLabel("to"));
    row2->addWidget(dateTo);

    row2->addStretch(1);

    root->addLayout(row2);

    // Table (+ hidden Idx)
    table = new QTableWidget(this);
    table->setColumnCount(8);
    table->setHorizontalHeaderLabels({
        "Idx", "Last name", "First name", "Patronymic",
        "Address", "Birth date", "Email", "Phones"
    });
    table->setColumnHidden(0, true);

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);

    // Встроенную сортировку отключаем: сортируем сами (иначе ломает "пустые в конец")
    table->setSortingEnabled(false);
    table->horizontalHeader()->setSortIndicatorShown(true);

    root->addWidget(table, 1);
    setCentralWidget(w);

    auto refresh = [this]() {
        reloadTable();
        sortByColumn(currentSortColumn, currentSortOrder);
    };

    auto parseConn = [](const QString& text, PostgresConfig& cfg, QString& err) -> bool {
        const QStringList tokens = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for (const QString& t : tokens) {
            int eq = t.indexOf('=');
            if (eq <= 0) continue;
            const QString k = t.left(eq).trimmed();
            const QString v = t.mid(eq + 1).trimmed();
            if (k == "host") cfg.host = v.toStdString();
            else if (k == "port") {
                bool ok = false;
                int p = v.toInt(&ok);
                if (!ok || p < 1 || p > 65535) { err = "Bad port"; return false; }
                cfg.port = p;
            }
            else if (k == "dbname") cfg.dbname = v.toStdString();
            else if (k == "user") cfg.user = v.toStdString();
            else if (k == "password") cfg.password = v.toStdString();
        }
        if (cfg.dbname.empty()) { err = "dbname is required"; return false; }
        return true;
    };

    connect(storageBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        [this, refresh, parseConn](int idx) {

        if (idx == 0) {
            model.setStorageBackend(StorageBackend::File);
            model.loadFromFile();
            reloadTable();
            sortByColumn(currentSortColumn, currentSortOrder);
            return;
        }

        // PostgreSQL
        PostgresConfig cfg = model.getPostgresConfig();

        QString prefill = QString("host=%1 port=%2 dbname=%3 user=%4 password=%5")
            .arg(QString::fromStdString(cfg.host))
            .arg(cfg.port)
            .arg(QString::fromStdString(cfg.dbname))
            .arg(QString::fromStdString(cfg.user))
            .arg(QString::fromStdString(cfg.password));

        bool ok = false;
        QString input = QInputDialog::getText(
            this,
            "PostgreSQL connection",
            "Enter: host=... port=... dbname=... user=... password=...",
            QLineEdit::Normal,
            prefill,
            &ok
        );

        if (!ok) {
            // отменили — возвращаем File
            storageBox->blockSignals(true);
            storageBox->setCurrentIndex(0);
            storageBox->blockSignals(false);
            return;
        }

        QString perr;
        if (!parseConn(input, cfg, perr)) {
            QMessageBox::critical(this, "PostgreSQL", "Invalid config: " + perr);
            storageBox->blockSignals(true);
            storageBox->setCurrentIndex(0);
            storageBox->blockSignals(false);
            return;
        }

        model.setPostgresConfig(cfg);
        model.setStorageBackend(StorageBackend::Postgres);

        std::string err;
        if (!model.loadFromDatabase(&err)) {
            QMessageBox::critical(this, "PostgreSQL", "Connection/load failed:\n" + QString::fromStdString(err));
            // откат
            model.setStorageBackend(StorageBackend::File);
            model.loadFromFile();

            storageBox->blockSignals(true);
            storageBox->setCurrentIndex(0);
            storageBox->blockSignals(false);
        }

        reloadTable();
        sortByColumn(currentSortColumn, currentSortOrder);
    });


    // update on search/date
    connect(searchEdit, &QLineEdit::textChanged, this, [refresh](const QString&) { refresh(); });
    connect(cbUseDateFilter, &QCheckBox::toggled, this, [refresh](bool){ refresh(); });
    connect(dateFrom, &QDateEdit::dateChanged, this, [refresh](const QDate&){ refresh(); });
    connect(dateTo, &QDateEdit::dateChanged, this, [refresh](const QDate&){ refresh(); });

    // Column selection via combobox: выбираем столбец и сортируем Asc
    connect(columnBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        currentSortColumn = 1 + idx; // 0..6 -> 1..7
        currentSortOrder = Qt::AscendingOrder;
        sortByColumn(currentSortColumn, currentSortOrder);
    });

    // Клик по заголовку: повторный клик по тому же столбцу переключает порядок
    connect(table->horizontalHeader(), &QHeaderView::sectionClicked, this, [this](int logicalIndex) {
        if (logicalIndex < 1 || logicalIndex > 7) return; // ignore Idx and out-of-range

        if (logicalIndex == currentSortColumn) {
            currentSortOrder = (currentSortOrder == Qt::AscendingOrder)
                               ? Qt::DescendingOrder
                               : Qt::AscendingOrder;
        } else {
            currentSortColumn = logicalIndex;
            currentSortOrder = Qt::AscendingOrder;
            columnBox->setCurrentIndex(currentSortColumn - 1);
        }

        sortByColumn(currentSortColumn, currentSortOrder);
    });

    // sort buttons
    connect(ascBtn, &QPushButton::clicked, this, [this]() {
        currentSortOrder = Qt::AscendingOrder;
        sortByColumn(currentSortColumn, currentSortOrder);
    });
    connect(descBtn, &QPushButton::clicked, this, [this]() {
        currentSortOrder = Qt::DescendingOrder;
        sortByColumn(currentSortColumn, currentSortOrder);
    });

    // CRUD
    connect(addBtn, &QPushButton::clicked, this, [this]() {
        ContactDialog dlg(this);
        dlg.setWindowTitle("Add contact");
        if (dlg.exec() == QDialog::Accepted) {
            model.addContact(dlg.getContact());
            reloadTable();
            sortByColumn(currentSortColumn, currentSortOrder);
        }
    });

    connect(editBtn, &QPushButton::clicked, this, [this]() {
        int idx = selectedModelIndex();
        if (idx < 0) return;

        auto contacts = model.getContacts();
        if (idx >= (int)contacts.size()) return;

        ContactDialog dlg(this);
        dlg.setWindowTitle("Edit contact");
        dlg.setContact(contacts[idx]);

        if (dlg.exec() == QDialog::Accepted) {
            model.updateContact(idx, dlg.getContact());
            reloadTable();
            sortByColumn(currentSortColumn, currentSortOrder);
        }
    });

    connect(delBtn, &QPushButton::clicked, this, [this]() {
        int idx = selectedModelIndex();
        if (idx < 0) return;

        if (QMessageBox::question(this, "Delete", "Delete selected contact?") != QMessageBox::Yes)
            return;

        model.removeContact(idx);
        reloadTable();
        sortByColumn(currentSortColumn, currentSortOrder);
    });
}

bool MainWindow::matchesSearchAllFields(const Contact& c, const QString& q) const {
    const QString query = q.trimmed();
    if (query.isEmpty()) return true;

    QString blob;
    blob += norm(c.getLastName()) + " ";
    blob += norm(c.getFirstName()) + " ";
    blob += norm(c.getPatronymic()) + " ";
    blob += norm(c.getAddress()) + " ";
    blob += norm(c.getDateOfBirth()) + " ";
    blob += norm(c.getEmail()) + " ";
    blob += phonesToString(c);

    return blob.contains(query, Qt::CaseInsensitive);
}

bool MainWindow::matchesDateRange(const Contact& c) const {
    if (!cbUseDateFilter->isChecked()) return true;

    QString dobStr = norm(c.getDateOfBirth());
    if (dobStr.isEmpty()) return false;

    QDate d = QDate::fromString(dobStr, "yyyy-MM-dd");
    if (!d.isValid()) return false;

    QDate from = dateFrom->date();
    QDate to = dateTo->date();
    if (from > to) std::swap(from, to);

    return d >= from && d <= to;
}

void MainWindow::fillRow(int row, int modelIndex, const Contact& c) {
    table->setItem(row, 0, new QTableWidgetItem(QString::number(modelIndex)));
    table->setItem(row, 1, new QTableWidgetItem(norm(c.getLastName())));
    table->setItem(row, 2, new QTableWidgetItem(norm(c.getFirstName())));
    table->setItem(row, 3, new QTableWidgetItem(norm(c.getPatronymic())));
    table->setItem(row, 4, new QTableWidgetItem(norm(c.getAddress())));
    table->setItem(row, 5, new QTableWidgetItem(norm(c.getDateOfBirth())));
    table->setItem(row, 6, new QTableWidgetItem(norm(c.getEmail())));
    table->setItem(row, 7, new QTableWidgetItem(phonesToString(c)));
}

int MainWindow::selectedModelIndex() const {
    int row = table->currentRow();
    if (row < 0) return -1;

    auto* it = table->item(row, 0);
    if (!it) return -1;

    bool ok = false;
    int idx = it->text().toInt(&ok);
    return ok ? idx : -1;
}

void MainWindow::reloadTable() {
    const auto contacts = model.getContacts();
    const QString q = searchEdit->text();

    table->setRowCount(0);

    for (int i = 0; i < (int)contacts.size(); ++i) {
        const auto& c = contacts[i];
        if (!matchesSearchAllFields(c, q)) continue;
        if (!matchesDateRange(c)) continue;

        int row = table->rowCount();
        table->insertRow(row);
        fillRow(row, i, c);
    }
}

// Сортировка выбранного столбца: пустые значения всегда в конце
void MainWindow::sortByColumn(int col, Qt::SortOrder order) {
    if (!table || col <= 0 || col >= table->columnCount()) return;

    struct RowPack {
        QString key;
        bool empty = true;
        std::vector<QTableWidgetItem*> items;
    };

    const int rows = table->rowCount();
    const int cols = table->columnCount();

    std::vector<RowPack> packs;
    packs.reserve(rows);

    for (int r = 0; r < rows; ++r) {
        RowPack p;

        QTableWidgetItem* it = table->item(r, col);
        p.key = it ? it->text().trimmed() : "";
        p.empty = p.key.isEmpty();

        p.items.resize(cols);
        for (int c = 0; c < cols; ++c) {
            QTableWidgetItem* src = table->takeItem(r, c);
            if (!src) src = new QTableWidgetItem("");
            p.items[c] = src;
        }

        packs.push_back(std::move(p));
    }

    auto cmpText = [](const QString& a, const QString& b) {
        return QString::localeAwareCompare(a, b);
    };

    std::stable_sort(packs.begin(), packs.end(),
        [&](const RowPack& A, const RowPack& B) {
            // непустые сначала, пустые в конец
            if (A.empty != B.empty) return A.empty < B.empty; // false < true

            int r = cmpText(A.key, B.key);
            if (order == Qt::AscendingOrder) return r < 0;
            return r > 0;
        });

    table->setRowCount(0);

    for (const auto& p : packs) {
        int r = table->rowCount();
        table->insertRow(r);
        for (int c = 0; c < cols; ++c) {
            table->setItem(r, c, p.items[c]);
        }
    }

    table->horizontalHeader()->setSortIndicator(col, order);
}
