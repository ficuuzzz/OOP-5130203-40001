#ifndef CONSOLECONTROLLER_H
#define CONSOLECONTROLLER_H

#include "ContactModel.h"
#include "ConsoleView.h"

class ConsoleController {
private:
    ContactModel model;
    bool running;

public:
    ConsoleController(const std::string& filename = "contacts.txt");
    void run();

private:
    // Обработчики меню
    void handleAddContact();
    void handleViewContacts();
    void handleSearchContacts();
    void handleSortContacts();
    void handleEditContact();
    void handleDeleteContact();
    
    // Вспомогательные методы
    void showSortMenu();
    int selectContact(const std::vector<Contact>& contacts);
};

#endif