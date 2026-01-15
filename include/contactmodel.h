#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include "Contact.h"
#include <vector>
#include <string>
#include <algorithm>


enum class SortField {
    FIRST_NAME,
    LAST_NAME,
    EMAIL,
    DATE_OF_BIRTH
};

enum class StorageBackend {
    File,
    Postgres
};

struct PostgresConfig {
    std::string host = "localhost";
    int port = 5432;
    std::string dbname = "phonebook";
    std::string user = "postgres";
    std::string password = "postgres";
};


class ContactModel {
private:
    std::vector<Contact> contacts;
    std::string filename;
    StorageBackend backend = StorageBackend::File;
    PostgresConfig pg;

public:
    ContactModel(const std::string& file = "contacts.txt");

    bool addContact(const Contact& contact);
    bool removeContact(int index);
    bool updateContact(int index, const Contact& contact);

    std::vector<Contact> search(const std::string& query) const;
    void sortBy(SortField field);

    bool loadFromFile();
    bool saveToFile() const;

    std::vector<Contact> getContacts() const;
    int getContactCount() const;

    // НУЖНО для GUI: получить индекс контакта в модели (по ключу first+last+email)
    int indexOf(const Contact& contact) const;

    void printAll() const;

    void setStorageBackend(StorageBackend b);
    StorageBackend getStorageBackend() const;

    void setPostgresConfig(const PostgresConfig& cfg);
    PostgresConfig getPostgresConfig() const;

    // универсальные load/save (сами решают: файл или БД)
    bool load();
    bool save() const;

    // БД-методы
    bool loadFromDatabase(std::string* err = nullptr);
    bool saveToDatabase(std::string* err = nullptr) const;

private:
    bool contactExists(const Contact& contact) const;
    int findContactIndex(const Contact& contact) const;
    static bool comparisonStrings(const std::string& a, const std::string& b);
};

#endif
