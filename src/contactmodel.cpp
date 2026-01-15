#include "../include/ContactModel.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QVariant>


using namespace std;

// Конструктор
ContactModel::ContactModel(const string& file) : filename(file) {
    // При создании модели автоматически загружаем данные из файла
    backend = StorageBackend::File;
    loadFromFile();
}

// Добавление контакта
bool ContactModel::addContact(const Contact& contact) {
    // Проверяем валидность контакта
    if (!contact.isValid()) {
        cout << "Error: Contact is not valid" << endl;
        return false;
    }

    // Проверяем нет ли уже такого контакта
    if (contactExists(contact)) {
        cout << "Error: Contact already exists" << endl;
        return false;
    }

    // Добавляем контакт
    contacts.push_back(contact);

    // Сохраняем изменения в файл
    if (save()) {
        cout << "Contact added successfully!" << endl;
        return true;
    } else {
        cout << "Error: Failed to save contact to file" << endl;
        // Откатываем изменения если не удалось сохранить
        contacts.pop_back();
        return false;
    }
}

// Удаление контакта по индексу
bool ContactModel::removeContact(int index) {
    if (index < 0 || index >= contacts.size()) {
        cout << "Error: Invalid contact index" << endl;
        return false;
    }

    // Сохраняем копию для сообщения
    Contact removed = contacts[index];

    // Удаляем контакт
    contacts.erase(contacts.begin() + index);

    // Сохраняем изменения
    if (save()) {
        cout << "Contact " << removed.getFirstName() << " "
             << removed.getLastName() << " removed successfully!" << endl;
        return true;
    } else {
        cout << "Error: Failed to save changes to file" << endl;
        // Восстанавливаем контакт если не удалось сохранить
        contacts.insert(contacts.begin() + index, removed);
        return false;
    }
}

// Обновление контакта
bool ContactModel::updateContact(int index, const Contact& contact) {
    if (index < 0 || index >= contacts.size()) {
        cout << "Error: Invalid contact index" << endl;
        return false;
    }

    if (!contact.isValid()) {
        cout << "Error: New contact data is not valid" << endl;
        return false;
    }

    // Сохраняем старую версию для отката
    Contact oldContact = contacts[index];

    // Обновляем контакт
    contacts[index] = contact;

    // Сохраняем изменения
    if (save()) {
        cout << "Contact updated successfully!" << endl;
        return true;
    } else {
        cout << "Error: Failed to save changes to file" << endl;
        // Откатываем изменения
        contacts[index] = oldContact;
        return false;
    }
}

// Поиск контактов
vector<Contact> ContactModel::search(const string& query) const {
    vector<Contact> results;
    string lowerQuery = query;

    // Преобразуем запрос в нижний регистр для регистронезависимого поиска
    transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    for (const auto& contact : contacts) {
        // Ищем в имени, фамилии, email и телефонах
        string firstName = contact.getFirstName();
        string lastName = contact.getLastName();
        string email = contact.getEmail();

        transform(firstName.begin(), firstName.end(), firstName.begin(), ::tolower);
        transform(lastName.begin(), lastName.end(), lastName.begin(), ::tolower);
        transform(email.begin(), email.end(), email.begin(), ::tolower);

        // Проверяем совпадения
        if (firstName.find(lowerQuery) != string::npos ||
            lastName.find(lowerQuery) != string::npos ||
            email.find(lowerQuery) != string::npos) {
            results.push_back(contact);
        } else {
            // Ищем в телефонах
            for (const auto& phone : contact.getPhoneNumbers()) {
                string phoneLower = phone;
                transform(phoneLower.begin(), phoneLower.end(), phoneLower.begin(), ::tolower);
                if (phoneLower.find(lowerQuery) != string::npos) {
                    results.push_back(contact);
                    break;
                }
            }
        }
    }

    return results;
}

// Сортировка контактов
void ContactModel::sortBy(SortField field) {
    switch (field) {
        case SortField::FIRST_NAME:
            sort(contacts.begin(), contacts.end(),
                [](const Contact& a, const Contact& b) {
                    return ContactModel::comparisonStrings(
                        a.getFirstName(), b.getFirstName());
                });
            break;

        case SortField::LAST_NAME:
            sort(contacts.begin(), contacts.end(),
                [](const Contact& a, const Contact& b) {
                    return ContactModel::comparisonStrings(
                        a.getLastName(), b.getLastName());
                });
            break;

        case SortField::EMAIL:
            sort(contacts.begin(), contacts.end(),
                [](const Contact& a, const Contact& b) {
                    return ContactModel::comparisonStrings(
                        a.getEmail(), b.getEmail());
                });
            break;

        case SortField::DATE_OF_BIRTH:
            sort(contacts.begin(), contacts.end(),
                [](const Contact& a, const Contact& b) {
                    return ContactModel::comparisonStrings(
                        a.getDateOfBirth(), b.getDateOfBirth());
                });
            break;
    }

    // Сохраняем отсортированный список
    save();
}

// Преобразование строчек в нижний регистр и их сравнение
bool ContactModel::comparisonStrings(const std::string& a, const std::string& b){
    if (a.empty() && b.empty()) return false;
    if (a.empty()) return false;
    if (b.empty()) return true;

    string aLower = a;
    string bLower = b;
    transform(aLower.begin(), aLower.end(), aLower.begin(), ::tolower);
    transform(bLower.begin(), bLower.end(), bLower.begin(), ::tolower);

    return aLower < bLower;

}

// Загрузка из файла
bool ContactModel::loadFromFile() {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    contacts.clear();
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue;

        QStringList parts = line.split('|');
        if (parts.size() < 7) continue;

        Contact c(
            parts[0].toStdString(),
            parts[1].toStdString(),
            parts[2].toStdString(),
            parts[3].toStdString(),
            parts[4].toStdString(),
            parts[5].toStdString()
        );

        QStringList phones = parts[6].split(',', Qt::SkipEmptyParts);
        for (const QString& p : phones)
            c.addPhoneNumber(p.toStdString());

        contacts.push_back(c);
    }

    return true;
}


// Сохранение в файл
bool ContactModel::saveToFile() const {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);

    for (const auto& c : contacts) {
        out << QString::fromStdString(c.getFirstName()) << "|"
            << QString::fromStdString(c.getLastName()) << "|"
            << QString::fromStdString(c.getPatronymic()) << "|"
            << QString::fromStdString(c.getAddress()) << "|"
            << QString::fromStdString(c.getDateOfBirth()) << "|"
            << QString::fromStdString(c.getEmail()) << "|";

        const auto& phones = c.getPhoneNumbers();
        for (size_t i = 0; i < phones.size(); ++i) {
            out << QString::fromStdString(phones[i]);
            if (i + 1 < phones.size()) out << ",";
        }
        out << "\n";
    }
    return true;
}

void ContactModel::setStorageBackend(StorageBackend b) {
    backend = b;
}

StorageBackend ContactModel::getStorageBackend() const {
    return backend;
}

void ContactModel::setPostgresConfig(const PostgresConfig& cfg) {
    pg = cfg;
}

PostgresConfig ContactModel::getPostgresConfig() const {
    return pg;
}

bool ContactModel::load() {
    if (backend == StorageBackend::File) return loadFromFile();
    std::string err;
    return loadFromDatabase(&err);
}

bool ContactModel::save() const {
    if (backend == StorageBackend::File) return saveToFile();
    std::string err;
    return saveToDatabase(&err);
}

static bool ensureSchema(QSqlDatabase& db, std::string* err) {
    QSqlQuery q(db);

    if (!q.exec(
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id SERIAL PRIMARY KEY,"
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL,"
        "patronymic TEXT,"
        "address TEXT,"
        "date_of_birth TEXT,"
        "email TEXT NOT NULL)"
    )) {
        if (err) *err = q.lastError().text().toStdString();
        return false;
    }

    if (!q.exec(
        "CREATE TABLE IF NOT EXISTS phones ("
        "id SERIAL PRIMARY KEY,"
        "contact_id INTEGER NOT NULL REFERENCES contacts(id) ON DELETE CASCADE,"
        "phone TEXT NOT NULL)"
    )) {
        if (err) *err = q.lastError().text().toStdString();
        return false;
    }

    return true;
}

static bool openPg(QSqlDatabase& db, const PostgresConfig& cfg, std::string* err) {
    const QString connName = "phonebook_pg_conn";

    if (QSqlDatabase::contains(connName)) {
        db = QSqlDatabase::database(connName);
    } else {
        db = QSqlDatabase::addDatabase("QPSQL", connName);
    }

    db.setHostName(QString::fromStdString(cfg.host));
    db.setPort(cfg.port);
    db.setDatabaseName(QString::fromStdString(cfg.dbname));
    db.setUserName(QString::fromStdString(cfg.user));
    db.setPassword(QString::fromStdString(cfg.password));

    if (!db.open()) {
        if (err) *err = db.lastError().text().toStdString();
        return false;
    }
    return true;
}

bool ContactModel::saveToDatabase(std::string* err) const {
    QSqlDatabase db;
    if (!openPg(db, pg, err)) return false;
    if (!ensureSchema(db, err)) return false;

    if (!db.transaction()) {
        if (err) *err = db.lastError().text().toStdString();
        return false;
    }

    QSqlQuery q(db);

    // как файл: перезаписываем всё содержимое БД под текущую модель
    if (!q.exec("TRUNCATE TABLE phones RESTART IDENTITY")) {
        db.rollback();
        if (err) *err = q.lastError().text().toStdString();
        return false;
    }
    if (!q.exec("TRUNCATE TABLE contacts RESTART IDENTITY CASCADE")) {
        db.rollback();
        if (err) *err = q.lastError().text().toStdString();
        return false;
    }

    QSqlQuery insC(db);
    insC.prepare(
        "INSERT INTO contacts(first_name,last_name,patronymic,address,date_of_birth,email) "
        "VALUES(:fn,:ln,:pat,:addr,:dob,:email) RETURNING id"
    );

    QSqlQuery insP(db);
    insP.prepare(
        "INSERT INTO phones(contact_id, phone) VALUES(:cid,:ph)"
    );

    for (const auto& c : contacts) {
        insC.bindValue(":fn", QString::fromStdString(c.getFirstName()));
        insC.bindValue(":ln", QString::fromStdString(c.getLastName()));
        insC.bindValue(":pat", QString::fromStdString(c.getPatronymic()));
        insC.bindValue(":addr", QString::fromStdString(c.getAddress()));
        insC.bindValue(":dob", QString::fromStdString(c.getDateOfBirth()));
        insC.bindValue(":email", QString::fromStdString(c.getEmail()));

        if (!insC.exec() || !insC.next()) {
            db.rollback();
            if (err) *err = insC.lastError().text().toStdString();
            return false;
        }

        const int contactId = insC.value(0).toInt();

        for (const auto& ph : c.getPhoneNumbers()) {
            insP.bindValue(":cid", contactId);
            insP.bindValue(":ph", QString::fromStdString(ph));
            if (!insP.exec()) {
                db.rollback();
                if (err) *err = insP.lastError().text().toStdString();
                return false;
            }
        }
    }

    if (!db.commit()) {
        if (err) *err = db.lastError().text().toStdString();
        return false;
    }
    return true;
}

bool ContactModel::loadFromDatabase(std::string* err) {
    QSqlDatabase db;
    if (!openPg(db, pg, err)) return false;
    if (!ensureSchema(db, err)) return false;

    contacts.clear();

    QSqlQuery qc(db);
    if (!qc.exec("SELECT id, first_name,last_name,patronymic,address,date_of_birth,email FROM contacts ORDER BY id")) {
        if (err) *err = qc.lastError().text().toStdString();
        return false;
    }

    QSqlQuery qp(db);
    qp.prepare("SELECT phone FROM phones WHERE contact_id = :cid ORDER BY id");

    while (qc.next()) {
        const int id = qc.value(0).toInt();

        Contact c(
            qc.value(1).toString().toStdString(),
            qc.value(2).toString().toStdString(),
            qc.value(3).toString().toStdString(),
            qc.value(4).toString().toStdString(),
            qc.value(5).toString().toStdString(),
            qc.value(6).toString().toStdString()
        );

        qp.bindValue(":cid", id);
        if (!qp.exec()) {
            if (err) *err = qp.lastError().text().toStdString();
            return false;
        }
        while (qp.next()) {
            c.addPhoneNumber(qp.value(0).toString().toStdString());
        }

        contacts.push_back(c);
    }

    return true;
}


// Геттеры
vector<Contact> ContactModel::getContacts() const {
    return contacts;
}


int ContactModel::getContactCount() const {
    return contacts.size();
}

// Вывод всех контактов (для отладки)
void ContactModel::printAll() const {
    cout << "=== All Contacts (" << contacts.size() << ") ===" << endl;
    for (size_t i = 0; i < contacts.size(); ++i) {
        cout << "[" << i << "] ";
        contacts[i].print();
    }
}

// Приватные вспомогательные методы

// Проверка существования контакта
bool ContactModel::contactExists(const Contact& contact) const {
    return findContactIndex(contact) != -1;
}

// Поиск индекса контакта
int ContactModel::findContactIndex(const Contact& contact) const {
    for (size_t i = 0; i < contacts.size(); ++i) {
        const Contact& existing = contacts[i];
        if (existing.getFirstName() == contact.getFirstName() &&
            existing.getLastName() == contact.getLastName() &&
            existing.getEmail() == contact.getEmail()) {
            return i;
        }
    }
    return -1;
}
