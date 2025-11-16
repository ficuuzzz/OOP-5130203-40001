#include "../include/ConsoleView.h"
#include <iostream>
#include <limits>

using namespace std;

void ConsoleView::printMenu() {
    clearScreen();
    printHeader("PHONE BOOK MANAGER");
    
    cout << "1. Add Contact" << endl;
    cout << "2. View All Contacts" << endl;
    cout << "3. Search Contacts" << endl;
    cout << "4. Sort Contacts" << endl;
    cout << "5. Edit Contact" << endl;
    cout << "6. Delete Contact" << endl;
    cout << "7. Exit" << endl;
    cout << "-------------------" << endl;
}

void ConsoleView::printContacts(const vector<Contact>& contacts) {
    if (contacts.empty()) {
        cout << "No contacts found." << endl;
        return;
    }
    
    cout << "=== Contacts (" << contacts.size() << ") ===" << endl;
    for (size_t i = 0; i < contacts.size(); ++i) {
        printContact(contacts[i], i);
    }
}

void ConsoleView::printContact(const Contact& contact, int index) {
    if (index >= 0) {
        cout << "[" << index << "] ";
    }
    
    cout << contact.getLastName() << " " << contact.getFirstName();
    if (!contact.getPatronymic().empty()) {
        cout << " " << contact.getPatronymic();
    }
    cout << endl;
    
    if (!contact.getAddress().empty()) {
        cout << "   Address: " << contact.getAddress() << endl;
    }
    if (!contact.getDateOfBirth().empty()) {
        cout << "   Birth Date: " << contact.getDateOfBirth() << endl;
    }
    if (!contact.getEmail().empty()) {
        cout << "   Email: " << contact.getEmail() << endl;
    }
    
    const auto& phones = contact.getPhoneNumbers();
    if (!phones.empty()) {
        cout << "   Phones: ";
        for (size_t i = 0; i < phones.size(); ++i) {
            cout << phones[i];
            if (i < phones.size() - 1) cout << ", ";
        }
        cout << endl;
    }
    cout << endl;
}

string ConsoleView::getStringInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return input;
}

int ConsoleView::getIntInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number." << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    return value;
}

Contact ConsoleView::getContactInput() {
    Contact contact;
    
    cout << "=== Enter Contact Details ===" << endl;
    
    // Ввод обязательных полей с повторением при ошибках
    while (true) {
        try {
            string firstName = getStringInput("First Name*: ");
            contact.setFirstName(firstName);
            break;
        } catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again." << endl;
        }
    }
    
    while (true) {
        try {
            string lastName = getStringInput("Last Name*: ");
            contact.setLastName(lastName);
            break;
        } catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again." << endl;
        }
    }
    
    // Необязательные поля
    // Ввод отчества
    while (true) {
        string patronymic = getStringInput("Patronymic: ");
        // Если пользователь нажал Enter - пропускаем 
        if (patronymic.empty()) {
            cout << "Patronymic skipped." << endl;
            break;
        }
        // Пробуем установить отчество
        try {
            contact.setPatronymic(patronymic);
            break; 
        } catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again or press Enter to skip patronymic." << endl;
        }
    }

    // Ввод адреса
    string address = getStringInput("Address: ");
    contact.setAddress(address);

    // Ввод даты
    while (true) {
        string dob = getStringInput("Birth Date (YYYY-MM-DD): ");
        // Если пользователь нажал Enter - пропускаем дату
        if (dob.empty()) {
            cout << "Birth date skipped." << endl;
            break;
        }
        // Пробуем установить дату
        try {
            contact.setDateOfBirth(dob);
            break; 
        } catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again or press Enter to skip birth date." << endl;
        }
    }

    // Вводе E-Mail
    while (true) {
        string email = getStringInput("Email: ");
        // Если пользователь нажал Enter - пропускаем 
        if (email.empty()) {
            cout << "Email skipped." << endl;
            break;
        }
        // Пробуем установить почту
        try {
            contact.setEmail(email);
            break; 
        } catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again or press Enter to skip email." << endl;
        }
    }

    // Ввод телефонных номеров
    cout << "Enter phone numbers (empty line to finish):" << endl;
    while (true) {
        string phone = getStringInput("Phone: ");
        if (phone.empty()) break;
        
        try {
            contact.addPhoneNumber(phone);
            cout << "Phone number added successfully!" << endl;
        } catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again or press Enter to skip." << endl;
        }
    }
    
    return contact;
}

void ConsoleView::clearScreen() {
    // Простой способ очистки экрана (работает в большинстве терминалов)
    cout << "\033[2J\033[1;1H";
}

void ConsoleView::printHeader(const string& title) {
    cout << "=================================" << endl;
    cout << "    " << title << endl;
    cout << "=================================" << endl;
}

void ConsoleView::waitForEnter() {
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}