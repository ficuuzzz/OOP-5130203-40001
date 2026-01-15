#include <QApplication>
#include "MainWindow.h"
#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QCoreApplication>


int main(int argc, char *argv[]) {
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");
    qDebug() << "SQL drivers:" << QSqlDatabase::drivers();

    QApplication app(argc, argv);

    MainWindow w("data/phonebook.txt");
    w.show();

    return app.exec();
}
