#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow w("data/phonebook.txt");
    w.show();

    return app.exec();
}
