#include "../include/ConsoleController.h"
#include <iostream>

using namespace std;

ConsoleController::ConsoleController(const string& filename) 
    : model(filename), running(true) {
}

void ConsoleController::run() {
    while (running) {
        ConsoleView::printMenu();
        
        int choice = ConsoleView::getIntInput("Select option: ");
        
        switch (choice) {
            case 1:
                handleAddContact();
                break;
            case 2:
                handleViewContacts();
                break;
            case 3:
                handleSearchContacts();
                break;
            case 4:
                handleSortContacts();
                break;
            case 5:
                handleEditContact();
                break;
            case 6:
                handleDeleteContact();
                break;
            case 7:
                running = false;
                cout << "Goodbye!" << endl;
                break;
            default:
                cout << "Invalid option. Please try again." << endl;
                ConsoleView::waitForEnter();
                break;
        }
    }
}

void ConsoleController::handleAddContact() {
    ConsoleView::clearScreen();
    ConsoleView::printHeader("ADD CONTACT");
    
    Contact contact = ConsoleView::getContactInput();
    
    if (model.addContact(contact)) {
    } else {
        cout << "Failed to add contact." << endl;
    }
    
    ConsoleView::waitForEnter();
}

void ConsoleController::handleViewContacts() {
    ConsoleView::clearScreen();
    ConsoleView::printHeader("ALL CONTACTS");
    
    auto contacts = model.getContacts();
    ConsoleView::printContacts(contacts);
    
    ConsoleView::waitForEnter();
}

void ConsoleController::handleSearchContacts() {
    ConsoleView::clearScreen();
    ConsoleView::printHeader("SEARCH CONTACTS");
    
    string query = ConsoleView::getStringInput("Enter search query: ");
    auto results = model.search(query);
    
    cout << "Found " << results.size() << " contacts:" << endl;
    ConsoleView::printContacts(results);
    
    ConsoleView::waitForEnter();
}

void ConsoleController::handleSortContacts() {
    ConsoleView::clearScreen();
    ConsoleView::printHeader("SORT CONTACTS");
    
    showSortMenu();
    int choice = ConsoleView::getIntInput("Select sort field: ");
    
    SortField field;
    switch (choice) {
        case 1: field = SortField::FIRST_NAME; break;
        case 2: field = SortField::LAST_NAME; break;
        case 3: field = SortField::EMAIL; break;
        case 4: field = SortField::DATE_OF_BIRTH; break;
        default:
            cout << "Invalid choice." << endl;
            ConsoleView::waitForEnter();
            return;
    }
    
    model.sortBy(field);
    cout << "Contacts sorted successfully!" << endl;
    
    ConsoleView::waitForEnter();
}

void ConsoleController::handleEditContact() {
    ConsoleView::clearScreen();
    ConsoleView::printHeader("EDIT CONTACT");
    
    auto contacts = model.getContacts();
    if (contacts.empty()) {
        cout << "No contacts available to edit." << endl;
        ConsoleView::waitForEnter();
        return;
    }
    
    ConsoleView::printContacts(contacts);
    int index = selectContact(contacts);
    
    if (index >= 0) {
        cout << "Editing contact:" << endl;
        ConsoleView::printContact(contacts[index], index);
        
        Contact newContact = ConsoleView::getContactInput();
        
        if (model.updateContact(index, newContact)) {
            cout << "Contact updated successfully!" << endl;
        } else {
            cout << "Failed to update contact." << endl;
        }
    }
    
    ConsoleView::waitForEnter();
}

void ConsoleController::handleDeleteContact() {
    ConsoleView::clearScreen();
    ConsoleView::printHeader("DELETE CONTACT");
    
    auto contacts = model.getContacts();
    if (contacts.empty()) {
        cout << "No contacts available to delete." << endl;
        ConsoleView::waitForEnter();
        return;
    }
    
    ConsoleView::printContacts(contacts);
    int index = selectContact(contacts);
    
    if (index >= 0) {
        cout << "Are you sure you want to delete this contact? (y/n): ";
        string confirmation;
        getline(cin, confirmation);
        
        if (confirmation == "y" || confirmation == "Y") {
            if (model.removeContact(index)) {
                // ...
            } else {
                cout << "Failed to delete contact." << endl;
            }
        } else {
            cout << "Deletion cancelled." << endl;
        }
    }
    
    ConsoleView::waitForEnter();
}

void ConsoleController::showSortMenu() {
    cout << "Sort by:" << endl;
    cout << "1. First Name" << endl;
    cout << "2. Last Name" << endl;
    cout << "3. Email" << endl;
    cout << "4. Birth Date" << endl;
}

int ConsoleController::selectContact(const vector<Contact>& contacts) {
    int index = ConsoleView::getIntInput("Enter contact number: ");
    
    if (index < 0 || index >= static_cast<int>(contacts.size())) {
        cout << "Invalid contact number." << endl;
        return -1;
    }
    
    return index;
}