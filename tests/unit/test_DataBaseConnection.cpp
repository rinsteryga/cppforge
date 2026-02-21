#include "../../src/core/include/utils/EnvLoader.hpp"

#include <QtTest>
#include <QtSql/QSqlDatabase>

namespace cppforge {
    namespace data {
        QSqlDatabase connectDatabase();
    }
}

class TestDataBaseConnection : public QObject
{
    Q_OBJECT

private:
    void test_DataBaseConnection()
    {
        QSqlDatabase db = cppforge::data::connectDatabase();
        
        QVERIFY(db.isValid());
        QVERIFY(db.isOpen());
        QCOMPARE(db.databaseName(), QString("cppforge"));
    }
};

QTEST_MAIN(TestDataBaseConnection)

#include "test_DataBaseConnection.moc"
