#include "repositories/DataBaseConnection.hpp"
#include "../core/include/utils/EnvLoader.hpp"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QProcessEnvironment>
#include <QDebug>

namespace cppforge::data
{
    QSqlDatabase connectDatabase()
    {
        // Загружаем переменные из .env файла
        
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        
        // Получаем настройки из переменных окружения
        QString host = env.value("PG_HOST");
        QString port = env.value("PG_PORT");
        QString dbName = env.value("PG_DB");
        QString user = env.value("PG_USER");
        QString password = env.value("PG_PASSWORD");
        
        // Проверяем обязательные параметры
        if (host.isEmpty() || port.isEmpty() || dbName.isEmpty() || user.isEmpty())
        {
            qCritical() << "Missing PostgreSQL environment variables";
            qCritical() << "PG_HOST:" << (host.isEmpty() ? "MISSING" : host);
            qCritical() << "PG_PORT:" << (port.isEmpty() ? "MISSING" : port);
            qCritical() << "PG_DB:" << (dbName.isEmpty() ? "MISSING" : dbName);
            qCritical() << "PG_USER:" << (user.isEmpty() ? "MISSING" : user);
            return QSqlDatabase();
        }
        
        // Создаем подключение
        QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
        db.setHostName(host);
        db.setPort(port.toInt());
        db.setDatabaseName(dbName);
        db.setUserName(user);
        db.setPassword(password);
        
        // Открываем подключение
        if (!db.open())
        {
            qCritical() << "Database connection failed:" << db.lastError().text();
            return QSqlDatabase();
        }
        
        qDebug() << "Connected to PostgreSQL database:" << dbName;
        return db;
    }
}