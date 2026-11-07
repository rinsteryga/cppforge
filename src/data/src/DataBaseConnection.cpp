#include "../../core/include/utils/EnvLoader.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

namespace cppforge
{
    namespace data
    {
        QSqlDatabase connectDatabase()
        {
            QStringList envPaths = {QCoreApplication::applicationDirPath() + "/.env",
                                    QCoreApplication::applicationDirPath() + "/../.env",
                                    QDir::currentPath() + "/.env",
                                    QDir::currentPath() + "/../.env",
                                    QDir::currentPath() + "/../../.env",
#ifdef Q_OS_LINUX
                                    "/etc/cppforge/.env",
                                    QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/.env"
#endif
            };

            bool loaded = false;
            for (const QString &path : envPaths)
            {
                if (cppforge::utils::loadEnvFile(path))
                {
                    loaded = true;
                    break;
                }
            }

            if (!loaded)
            {
                qWarning() << "Could not load .env file from standard locations. Defaulting to environment variables.";
            }

            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

            auto getVal = [&](const char *name, const QString &defVal) -> QString
            {
                QByteArray qEnv = qgetenv(name);
                if (!qEnv.isEmpty())
                {
                    return QString::fromLocal8Bit(qEnv);
                }
                if (env.contains(name) && !env.value(name).isEmpty())
                {
                    return env.value(name);
                }
                return defVal;
            };

            QString host = getVal("PG_HOST", "127.0.0.1");
            QString port = getVal("PG_PORT", "5432");
            QString dbName = getVal("PG_DB", "cppforge");
            QString user = getVal("PG_USER", "cppforge");
            QString password = getVal("PG_PASSWORD", "secret_password_123");

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
