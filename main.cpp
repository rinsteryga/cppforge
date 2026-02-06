#include "src/gui/include/MainWindow.hpp"
#include "src/data/repositories/DataBaseConnection.hpp"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include "../core/include/utils/EnvLoader.hpp"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    cppforge::utils::loadEnvFile("../.env");
    
    QSqlDatabase db = cppforge::data::connectDatabase();
    
    if (!db.isOpen())
    {
        QMessageBox::critical(nullptr, "Database Error",
                             "Cannot connect to database.\n"
                             "Check your .env file and PostgreSQL server.");
        return 1;
    }
    
    qDebug() << "Database connected:" << db.databaseName();
    
    // Создаем главное окно
    MainWindow window(db);
    window.show();
    
    return app.exec();
}