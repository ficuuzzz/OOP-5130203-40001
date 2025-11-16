#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include "Contact.h"
#include <vector>
#include <string>
#include <algorithm>

// Перечисление для полей сортировки
enum class SortField {
    FIRST_NAME,
    LAST_NAME, 
    EMAIL,
    DATE_OF_BIRTH
};

class ContactModel {
private:
    std::vector<Contact> contacts;
    std::string filename;  // имя файла для сохранения/загрузки
    
public:
    // Конструктор
    ContactModel(const std::string& file = "contacts.txt");
    
    // Основные операции с контактами
    bool addContact(const Contact& contact);
    bool removeContact(int index);
    bool updateContact(int index, const Contact& contact);
    
    // Поиск и фильтрация
    std::vector<Contact> search(const std::string& query) const;
    
    // Сортировка
    void sortBy(SortField field);

    // Работа с файлами
    bool loadFromFile();
    bool saveToFile() const;
    
    // Геттеры
    std::vector<Contact> getContacts() const;
    int getContactCount() const;
    
    // Вспомогательные методы
    void printAll() const;  // для отладки

private:
    // Приватные вспомогательные методы
    bool contactExists(const Contact& contact) const;
    int findContactIndex(const Contact& contact) const;
    static bool comparisonStrings(const std::string& a, const std::string& b);

};

#endif