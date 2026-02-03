#include "repositories/DataBaseConnection.hpp"
#include "../core/include/utils/EnvLoader.hpp"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QProcessEnvironment>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QDir>
#include <QCoreApplication>

namespace cppforge
{
    namespace data
    {
        QSqlDatabase connectDatabase(bool initSchema)
        {
            // Загружаем .env файл из текущей директории
            QString projectDir = QCoreApplication::applicationDirPath();
            QString envPath = projectDir + "/../../.env"; // Для папки build
            
            // Пробуем несколько возможных путей
            if (!cppforge::utils::loadEnvFile(envPath))
            {
                // Пробуем из текущей директории
                if (!cppforge::utils::loadEnvFile(".env"))
                {
                    // Пробуем относительный путь
                    if (!cppforge::utils::loadEnvFile("../../.env"))
                    {
                        qWarning() << "Cannot load .env file, trying system environment";
                    }
                }
            }

            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

            QString host = env.value("PG_HOST", "localhost");
            QString port = env.value("PG_PORT", "5432");
            QString dbName = env.value("PG_DB", "cppforge");
            QString user = env.value("PG_USER", "postgres");
            QString password = env.value("PG_PASSWORD", "");

            qDebug() << "Connecting to PostgreSQL:";
            qDebug() << "  Host:" << host;
            qDebug() << "  Port:" << port;
            qDebug() << "  Database:" << dbName;
            qDebug() << "  User:" << user;

            QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
            db.setHostName(host);
            db.setPort(port.toInt());
            db.setDatabaseName(dbName);
            db.setUserName(user);
            db.setPassword(password);

            if (!db.open())
            {
                qCritical() << "Database connection failed:" << db.lastError().text();
                return QSqlDatabase();
            }

            qDebug() << "Successfully connected to database:" << dbName;

            // Инициализация схемы
            if (initSchema)
            {
                initializeDatabaseSchema(db);
            }

            return db;
        }

        bool initializeDatabaseSchema(QSqlDatabase& db)
        {
            if (!db.isOpen())
            {
                qCritical() << "Database is not open";
                return false;
            }

            // Ищем файл schema.sql в разных местах
            QStringList possiblePaths;
            possiblePaths << "schema.sql"
                          << "src/data/migrations/schema.sql"
                          << "../../src/data/migrations/schema.sql"
                          << "../../../src/data/migrations/schema.sql";

            QFile schemaFile;
            QString foundPath;

            for (const QString& path : possiblePaths)
            {
                schemaFile.setFileName(path);
                if (schemaFile.exists())
                {
                    foundPath = path;
                    break;
                }
            }

            if (foundPath.isEmpty())
            {
                qCritical() << "Cannot find schema.sql file. Looked in:";
                for (const QString& path : possiblePaths)
                {
                    qCritical() << "  " << path;
                }
                return false;
            }

            if (!schemaFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                qCritical() << "Cannot open schema.sql file:" << foundPath;
                return false;
            }

            qDebug() << "Found schema.sql at:" << foundPath;

            QTextStream in(&schemaFile);
            QString sqlContent = in.readAll();
            schemaFile.close();

            QSqlQuery query(db);
            QStringList sqlStatements = sqlContent.split(';', Qt::SkipEmptyParts);

            bool success = true;
            int executed = 0;
            
            for (const QString& sql : sqlStatements)
            {
                QString trimmed = sql.trimmed();
                if (trimmed.isEmpty() || trimmed.startsWith("--"))
                    continue;

                if (!query.exec(trimmed))
                {
                    // Пропускаем ошибки "таблица уже существует"
                    if (!query.lastError().text().contains("already exists", Qt::CaseInsensitive))
                    {
                        qCritical() << "Failed to execute SQL:" << query.lastError().text();
                        qCritical() << "SQL:" << trimmed.left(200);
                        success = false;
                    }
                    else
                    {
                        qDebug() << "Table already exists, skipping...";
                    }
                }
                else
                {
                    executed++;
                }
            }

            if (success)
            {
                qDebug() << "Database schema initialized successfully. Executed" << executed << "statements";
            }

            return success;
        }

        bool checkDatabaseTables(QSqlDatabase& db)
        {
            if (!db.isOpen())
                return false;

            QStringList tables = db.tables();
            qDebug() << "Available tables:" << tables;
            
            bool hasUsers = tables.contains("users", Qt::CaseInsensitive);
            bool hasAllTables = tables.contains("users", Qt::CaseInsensitive) &&
                               tables.contains("projects", Qt::CaseInsensitive);
            
            return hasUsers;
        }
    } // namespace data
} // namespace cppforge