#include "../../src/core/include/entities/User.hpp"
#include "../../src/data/include/repositories/PgUserRepository.hpp"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtTest>

#include <chrono>

namespace
{
    struct LegacyDatabaseConfig
    {
        QString host;
        QString port;
        QString database;
        QString user;
        QString password;

        bool isComplete() const { return !host.isEmpty() && !port.isEmpty() && !database.isEmpty() && !user.isEmpty(); }
    };

    QString testValue(const char *testName, const char *legacyName)
    {
        const QString testValue = qEnvironmentVariable(testName);
        return testValue.isEmpty() ? qEnvironmentVariable(legacyName) : testValue;
    }

    LegacyDatabaseConfig legacyDatabaseConfig()
    {
        return {.host = testValue("TEST_PG_HOST", "PG_HOST"),
                .port = testValue("TEST_PG_PORT", "PG_PORT"),
                .database = testValue("TEST_PG_DB", "PG_DB"),
                .user = testValue("TEST_PG_USER", "PG_USER"),
                .password = testValue("TEST_PG_PASSWORD", "PG_PASSWORD")};
    }

    bool requireDatabaseTests()
    {
        return qEnvironmentVariableIntValue("CPPFORGE_REQUIRE_DB_TESTS") == 1;
    }

    void failOrSkip(const QString &message)
    {
        if (requireDatabaseTests())
        {
            QFAIL(qPrintable(message));
        }
        QSKIP(qPrintable(message));
    }
} // namespace

class TestDataBaseConnection : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QPSQL")))
        {
            failOrSkip(QStringLiteral("QPSQL driver is not available"));
        }

        m_config = legacyDatabaseConfig();
        if (!m_config.isComplete())
        {
            failOrSkip(QStringLiteral(
                "Set TEST_PG_* (or PG_*) variables pointing at cppforge_test to run the legacy database test"));
        }

        m_connectionName = QStringLiteral("cppforge_legacy_test_%1").arg(reinterpret_cast<quintptr>(this));
        m_database = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), m_connectionName);
        m_database.setHostName(m_config.host);
        m_database.setPort(m_config.port.toInt());
        m_database.setDatabaseName(m_config.database);
        m_database.setUserName(m_config.user);
        m_database.setPassword(m_config.password);
        m_database.setConnectOptions(QStringLiteral("connect_timeout=3"));

        if (!m_database.open())
        {
            failOrSkip(
                QStringLiteral("Unable to open the legacy test database: %1").arg(m_database.lastError().text()));
        }
    }

    void test_DataBaseConnection()
    {
        QVERIFY(m_database.isValid());
        QVERIFY(m_database.isOpen());
        QCOMPARE(m_database.databaseName(), m_config.database);
    }

    void test_PgUserRepository_insert()
    {
        QVERIFY(m_database.isOpen());

        cppforge::repositories::PgUserRepository repo(m_database);

        cppforge::entities::User newUser(0, "testuser_repo", "testuser_repo@example.com", "fakehash123", "fakesalt123",
                                         std::chrono::system_clock::now());

        QSqlQuery cleanup(m_database);
        QVERIFY(cleanup.prepare(QStringLiteral("DELETE FROM users WHERE email = :email")));
        cleanup.bindValue(QStringLiteral(":email"), QStringLiteral("testuser_repo@example.com"));
        QVERIFY(cleanup.exec());

        repo.save(newUser);

        auto fetchedUser = repo.findByEmail("testuser_repo@example.com");
        QVERIFY(fetchedUser.has_value());
        QCOMPARE(fetchedUser->getUsername(), QString("testuser_repo"));

        QSqlQuery remove(m_database);
        QVERIFY(remove.prepare(QStringLiteral("DELETE FROM users WHERE email = :email")));
        remove.bindValue(QStringLiteral(":email"), QStringLiteral("testuser_repo@example.com"));
        QVERIFY(remove.exec());
    }

    void cleanupTestCase()
    {
        if (m_database.isValid())
        {
            m_database.close();
            m_database = QSqlDatabase();
            QSqlDatabase::removeDatabase(m_connectionName);
        }
    }

private:
    LegacyDatabaseConfig m_config;
    QString m_connectionName;
    QSqlDatabase m_database;
};

QTEST_MAIN(TestDataBaseConnection)

#include "test_DataBaseConnection.moc"
