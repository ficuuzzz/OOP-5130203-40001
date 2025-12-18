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

class ContactModel {
private:
    std::vector<Contact> contacts;
    std::string filename;

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

private:
    bool contactExists(const Contact& contact) const;
    int findContactIndex(const Contact& contact) const;
    static bool comparisonStrings(const std::string& a, const std::string& b);
};

#endif
