#include "../../src/core/include/utils/EnvLoader.hpp"
#include "../../src/core/include/entities/User.hpp"
#include "../../src/data/repositories/PgUserRepository.hpp"

#include <QtTest>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

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
        db_ = cppforge::data::connectDatabase();
        
        QVERIFY(db_.isValid());
        QVERIFY(db_.isOpen());
        QCOMPARE(db_.databaseName(), QString("cppforge"));
    }

    void test_PgUserRepository_insert()
    {
        QVERIFY(db_.isOpen());

        cppforge::repositories::PgUserRepository repo(db_);
        
        cppforge::entities::User newUser(0, "testuser_repo", "testuser_repo@example.com", "fakehash123", std::chrono::system_clock::now());
        
        repo.save(newUser);

        auto fetchedUser = repo.findByEmail("testuser_repo@example.com");
        QVERIFY(fetchedUser.has_value());
        QCOMPARE(fetchedUser->getUsername(), QString("testuser_repo"));

        QSqlQuery q(db_);
        q.exec("DELETE FROM users WHERE email = 'testuser_repo@example.com'");
    }

    void cleanupTestCase()
    {
        if (db_.isOpen()) {
            db_.close();
        }
    }

private:
    QSqlDatabase db_;
};

QTEST_MAIN(TestDataBaseConnection)

#include "test_DataBaseConnection.moc"
