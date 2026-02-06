#include <QtTest>
#include <QFile>
#include "../../src/core/include/utils/EnvLoader.hpp"
#include "../../src/data/repositories/DataBaseConnection.hpp"

using namespace cppforge::utils;

class TestDatabaseConnection : public QObject 
{
    Q_OBJECT
private:
    void test_Connection()
    {
        cppforge::utils::loadEnvFile("../.env");
        QSqlDatabase db = cppforge::data::connectDatabase();
        QVERIFY(db.isOpen());
    }

};

QTEST_MAIN(TestDatabaseConnection)

#include "test_DatabaseConnection.moc"