#pragma once

#include <QtSql/QSqlDatabase>

namespace cppforge
{
    namespace data
    {
        QSqlDatabase connectDatabase(bool initSchema = true);
        bool initializeDatabaseSchema(QSqlDatabase& db);
        bool checkDatabaseTables(QSqlDatabase& db);
    }
}