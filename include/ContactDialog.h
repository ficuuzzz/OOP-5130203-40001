#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QListWidget>
#include <QPushButton>

#include "contact.h"

class ContactDialog : public QDialog {
    Q_OBJECT
public:
    explicit ContactDialog(QWidget* parent = nullptr);

    void setContact(const Contact& c);
    Contact getContact() const;

private slots:
    void addPhone();
    void removePhone();
    void onOk();

private:
    QLineEdit *lastEdit, *firstEdit, *patronEdit, *addressEdit, *emailEdit, *phoneEdit;
    QDateEdit *dobEdit;
    QListWidget *phonesList;
    QPushButton *addPhoneBtn, *removePhoneBtn, *okBtn, *cancelBtn;
};
