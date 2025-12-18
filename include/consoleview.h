#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#include "Contact.h"
#include <vector>
#include <string>

class ConsoleView {
public:
    // Основные методы отображения
    static void printMenu();
    static void printContacts(const std::vector<Contact>& contacts);
    static void printContact(const Contact& contact, int index = -1);
    
    // Методы ввода данных
    static std::string getStringInput(const std::string& prompt);
    static int getIntInput(const std::string& prompt);
    static Contact getContactInput();
    
    // Вспомогательные методы
    static void clearScreen();
    static void printHeader(const std::string& title);
    static void waitForEnter();
};

#endif
