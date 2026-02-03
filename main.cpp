#include "src/gui/include/MainWindow.hpp"
#include "src/data/repositories/DataBaseConnection.hpp"
#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include <QDebug>
#include <QPalette>
#include <QDateTime>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Устанавливаем стиль приложения
    app.setStyle(QStyleFactory::create("Fusion"));

    qDebug() << "Starting CppForge application at" << QDateTime::currentDateTime().toString();

    // Инициализируем БД
    QSqlDatabase db = cppforge::data::connectDatabase(true);

    if (!db.isOpen())
    {
        QMessageBox::critical(nullptr, "Database Error",
                             "Cannot connect to database. Please check:\n"
                             "1. PostgreSQL is running\n"
                             "2. .env file exists with correct settings\n"
                             "3. Database 'cppforge' exists\n\n"
                             "Error details have been printed to console.");
        return 1;
    }

    // Проверяем таблицы
    if (!cppforge::data::checkDatabaseTables(db))
    {
        QMessageBox::warning(nullptr, "Database Warning",
                            "Database schema may not be fully initialized.\n"
                            "Some features may not work correctly.");
    }

    qDebug() << "Database initialized successfully";

    // Создаем главное окно, передавая БД
    MainWindow window(db);
    window.show();

    qDebug() << "Main window shown";

    return app.exec();
}