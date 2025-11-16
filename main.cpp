#include <iostream>
#include <locale>
#include <windows.h>
#include "include/ConsoleController.h"

int main() {
    SetConsoleCP(1251);       // Устанавливаем кодировку ввода (Windows-1251)
    SetConsoleOutputCP(1251); // Устанавливаем кодировку вывода (Windows-1251)
    setlocale(LC_ALL, "Russian");

    std::cout << "Starting Phone Book Application..." << std::endl;
    
    // Создаем и запускаем контроллер
    ConsoleController controller("data/phonebook.txt");
    controller.run();
    
    std::cout << "Application finished." << std::endl;
    return 0;
}