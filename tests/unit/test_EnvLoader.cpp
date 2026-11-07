#include "../../src/core/include/utils/EnvLoader.hpp"

#include <QFile>
#include <QtTest>

using namespace cppforge::utils;

class TestEnvLoader : public QObject
{
    Q_OBJECT

private slots:
    void init()
    {
        for (const auto *name : {"PG_HOST", "PG_PORT", "PG_DB", "PG_USER", "PG_PASSWORD"})
        {
            qunsetenv(name);
        }
    }

    void cleanup() { init(); }

    void test_EnvLoader()
    {
        QString path = QFINDTESTDATA(".env.example");

        QVERIFY(loadEnvFile(path));

        QCOMPARE(qgetenv("PG_HOST"), "localhost");
        QCOMPARE(qgetenv("PG_PORT"), "5432");
        QCOMPARE(qgetenv("PG_DB"), "cppforge");
        QCOMPARE(qgetenv("PG_USER"), "your_username");
        QCOMPARE(qgetenv("PG_PASSWORD"), "your_password");
    }

    void test_LoadNonExistentFile() { QVERIFY(!loadEnvFile("non_existent_file.env")); }

    void test_VerifyVariablesAreSet()
    {
        QString path = QFINDTESTDATA(".env.example");

        QVERIFY(loadEnvFile(path));

        QVERIFY(!qgetenv("PG_HOST").isEmpty());
        QVERIFY(!qgetenv("PG_PORT").isEmpty());
        QVERIFY(!qgetenv("PG_DB").isEmpty());
        QVERIFY(!qgetenv("PG_USER").isEmpty());
        QVERIFY(!qgetenv("PG_PASSWORD").isEmpty());
    }

    void processEnvironmentWinsOverDotEnv()
    {
        qputenv("PG_HOST", "process-host");
        qputenv("PG_USER", "process-user");

        QString path = QFINDTESTDATA(".env.example");
        QVERIFY(loadEnvFile(path));

        QCOMPARE(qgetenv("PG_HOST"), "process-host");
        QCOMPARE(qgetenv("PG_USER"), "process-user");
        QCOMPARE(qgetenv("PG_DB"), "cppforge");
    }

    void loadingAnExistingFileSucceedsEvenWhenEverythingIsPreserved()
    {
        for (const auto *name : {"PG_HOST", "PG_PORT", "PG_DB", "PG_USER", "PG_PASSWORD"})
        {
            qputenv(name, "preset");
        }

        QString path = QFINDTESTDATA(".env.example");
        QVERIFY(loadEnvFile(path));
        QCOMPARE(qgetenv("PG_HOST"), "preset");
    }
};

QTEST_MAIN(TestEnvLoader)

#include "test_EnvLoader.moc"
