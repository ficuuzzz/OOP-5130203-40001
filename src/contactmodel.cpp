#include "../include/ContactModel.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// Конструктор
ContactModel::ContactModel(const string& file) : filename(file) {
    // При создании модели автоматически загружаем данные из файла
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
    if (saveToFile()) {
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
    if (saveToFile()) {
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
    if (saveToFile()) {
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
    saveToFile();
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
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Info: Could not open file " << filename 
             << " (it might not exist yet)" << endl;
        return false;
    }
    
    contacts.clear();  // Очищаем текущие контакты
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        // Простой парсинг - в реальном приложении нужно использовать CSV или JSON
        // Формат: Имя|Фамилия|Отчество|Адрес|ДатаРождения|Email|Телефоны
        stringstream ss(line);
        string firstName, lastName, patronymic, address, dateOfBirth, email, phonesStr;
        
        getline(ss, firstName, '|');
        getline(ss, lastName, '|');
        getline(ss, patronymic, '|');
        getline(ss, address, '|');
        getline(ss, dateOfBirth, '|');
        getline(ss, email, '|');
        getline(ss, phonesStr, '|');
        
        // Создаем контакт
        Contact contact(firstName, lastName, patronymic, address, dateOfBirth, email);
        
        // Парсим телефоны (разделены запятыми)
        stringstream phonesStream(phonesStr);
        string phone;
        while (getline(phonesStream, phone, ',')) {
            if (!phone.empty()) {
                try {
                    contact.addPhoneNumber(phone);
                } catch (const invalid_argument& e) {
                    cout << "Warning: Skipping invalid phone number '" << phone 
                         << "' for contact " << firstName << " " << lastName << endl;
                }
            }
        }
        
        contacts.push_back(contact);
    }
    
    file.close();
    cout << "Loaded " << contacts.size() << " contacts from " << filename << endl;
    return true;
}

// Сохранение в файл
bool ContactModel::saveToFile() const {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << " for writing" << endl;
        return false;
    }
    
    for (const auto& contact : contacts) {
        file << contact.getFirstName() << "|"
             << contact.getLastName() << "|"
             << contact.getPatronymic() << "|"
             << contact.getAddress() << "|"
             << contact.getDateOfBirth() << "|"
             << contact.getEmail() << "|";
        
        // Сохраняем телефоны через запятую
        const auto& phones = contact.getPhoneNumbers();
        for (size_t i = 0; i < phones.size(); ++i) {
            file << phones[i];
            if (i < phones.size() - 1) {
                file << ",";
            }
        }
        
        file << endl;
    }
    
    file.close();
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