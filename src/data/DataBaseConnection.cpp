#include "../core/include/utils/EnvLoader.hpp"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QProcessEnvironment>
#include <QDebug>

namespace cppforge
{
    namespace data
    {
        QSqlDatabase connectDatabase()
        {
            cppforge::utils::loadEnvFile("../../.env");

            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

            QString host = env.value("PG_HOST");
            QString port = env.value("PG_PORT");
            QString dbName = env.value("PG_DB");
            QString user = env.value("PG_USER");
            QString password = env.value("PG_PASSWORD");

            if (host.isEmpty() || port.isEmpty() || dbName.isEmpty() || user.isEmpty())
            {
                qCritical() << "Missing environment variables PG_*";
                return QSqlDatabase();
            }

            QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
            db.setHostName(host);
            db.setPort(port.toInt());
            db.setDatabaseName(dbName);
            db.setUserName(user);
            db.setPassword(password);

            if (!db.open())
            {
                qCritical() << "Database connection failed:" << db.lastError().text();
            }
            else
            {
                qDebug() << "Connected to database:" << dbName;
            }

            return db;
        }
    } // namespace data
} // namespace cppforge
