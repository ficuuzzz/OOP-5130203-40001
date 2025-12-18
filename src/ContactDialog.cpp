#include "ContactDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

ContactDialog::ContactDialog(QWidget* parent) : QDialog(parent) {
    setModal(true);
    setWindowTitle("Contact");

    auto* root = new QVBoxLayout(this);

    auto* form = new QFormLayout();
    lastEdit = new QLineEdit();
    firstEdit = new QLineEdit();
    patronEdit = new QLineEdit();
    addressEdit = new QLineEdit();
    emailEdit = new QLineEdit();

    dobEdit = new QDateEdit();
    dobEdit->setCalendarPopup(true);
    dobEdit->setDisplayFormat("yyyy-MM-dd");
    dobEdit->setMinimumDate(QDate(1900,1,1));
    dobEdit->setDate(QDate(1900,1,1)); // "пустая"

    form->addRow("Last name*:", lastEdit);
    form->addRow("First name*:", firstEdit);
    form->addRow("Patronymic:", patronEdit);
    form->addRow("Address:", addressEdit);
    form->addRow("Birth date:", dobEdit);
    form->addRow("Email*:", emailEdit);

    root->addLayout(form);

    phonesList = new QListWidget();
    root->addWidget(phonesList);

    auto* phRow = new QHBoxLayout();
    phoneEdit = new QLineEdit();
    phoneEdit->setPlaceholderText("phone...");
    addPhoneBtn = new QPushButton("Add phone");
    removePhoneBtn = new QPushButton("Remove phone");

    phRow->addWidget(phoneEdit, 1);
    phRow->addWidget(addPhoneBtn);
    phRow->addWidget(removePhoneBtn);
    root->addLayout(phRow);

    connect(addPhoneBtn, &QPushButton::clicked, this, &ContactDialog::addPhone);
    connect(removePhoneBtn, &QPushButton::clicked, this, &ContactDialog::removePhone);

    auto* btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    okBtn = new QPushButton("OK");
    cancelBtn = new QPushButton("Cancel");
    btnRow->addWidget(okBtn);
    btnRow->addWidget(cancelBtn);
    root->addLayout(btnRow);

    connect(okBtn, &QPushButton::clicked, this, &ContactDialog::onOk);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void ContactDialog::addPhone() {
    QString p = phoneEdit->text().trimmed();
    if (p.isEmpty()) return;
    phonesList->addItem(p);
    phoneEdit->clear();
}

void ContactDialog::removePhone() {
    auto* it = phonesList->currentItem();
    if (!it) return;
    delete it;
}

void ContactDialog::onOk() {
    if (lastEdit->text().trimmed().isEmpty() ||
        firstEdit->text().trimmed().isEmpty() ||
        emailEdit->text().trimmed().isEmpty()) {
        QMessageBox::critical(this, "Error", "Last name, First name, Email are required.");
        return;
    }
    if (phonesList->count() < 1) {
        QMessageBox::critical(this, "Error", "At least one phone is required.");
        return;
    }

    try {
        (void)getContact(); // проверка валидаторов (сеттеры могут бросить исключение)
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Validation error", e.what());
        return;
    }

    accept();
}

Contact ContactDialog::getContact() const {
    Contact c;
    c.setLastName(lastEdit->text().toStdString());
    c.setFirstName(firstEdit->text().toStdString());
    c.setPatronymic(patronEdit->text().toStdString());
    c.setAddress(addressEdit->text().toStdString());

    QDate d = dobEdit->date();
    if (d == QDate(1900,1,1)) c.setDateOfBirth("");
    else c.setDateOfBirth(d.toString("yyyy-MM-dd").toStdString());

    c.setEmail(emailEdit->text().toStdString());

    for (int i = 0; i < phonesList->count(); ++i) {
        c.addPhoneNumber(phonesList->item(i)->text().toStdString());
    }
    return c;
}

void ContactDialog::setContact(const Contact& c) {
    lastEdit->setText(QString::fromStdString(c.getLastName()));
    firstEdit->setText(QString::fromStdString(c.getFirstName()));
    patronEdit->setText(QString::fromStdString(c.getPatronymic()));
    addressEdit->setText(QString::fromStdString(c.getAddress()));
    emailEdit->setText(QString::fromStdString(c.getEmail()));

    const QString dob = QString::fromStdString(c.getDateOfBirth());
    if (dob.trimmed().isEmpty()) dobEdit->setDate(QDate(1900,1,1));
    else dobEdit->setDate(QDate::fromString(dob, "yyyy-MM-dd"));

    phonesList->clear();
    for (const auto& ph : c.getPhoneNumbers())
        phonesList->addItem(QString::fromStdString(ph));
}
