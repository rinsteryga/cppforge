#include <server/ServerConfig.hpp>

#include <QtTest>

#include <optional>
#include <string>

namespace
{
    const char *const environmentKeys[] = {"SERVER_HOST", "SERVER_PORT", "LOG_LEVEL", "CORS_ALLOWED_ORIGIN", "PG_HOST",
                                           "PG_PORT",     "PG_DB",       "PG_USER",   "PG_PASSWORD"};

    void clearEnvironment()
    {
        for (const auto *name : environmentKeys)
        {
            qunsetenv(name);
        }
    }

    void setValidDatabaseEnvironment()
    {
        qputenv("PG_HOST", "127.0.0.1");
        qputenv("PG_PORT", "55432");
        qputenv("PG_DB", "cppforge_test");
        qputenv("PG_USER", "cppforge");
        qputenv("PG_PASSWORD", "secret");
    }

    std::optional<std::string> configurationError()
    {
        try
        {
            cppforge::server::ServerConfig::fromEnvironment();
            return std::nullopt;
        }
        catch (const std::exception &error)
        {
            return std::string(error.what());
        }
    }
} // namespace

class TestServerConfig : public QObject
{
    Q_OBJECT

private slots:
    void init() { clearEnvironment(); }
    void cleanup() { clearEnvironment(); }

    void loadsDefaults()
    {
        setValidDatabaseEnvironment();

        const auto config = cppforge::server::ServerConfig::fromEnvironment();

        QCOMPARE(QString::fromStdString(config.host), QStringLiteral("127.0.0.1"));
        QCOMPARE(config.port, std::uint16_t(18080));
        QCOMPARE(QString::fromStdString(config.logLevel), QStringLiteral("info"));
        QVERIFY(config.allowedOrigin.empty());
        QCOMPARE(QString::fromStdString(config.database.database), QStringLiteral("cppforge_test"));
        QCOMPARE(config.database.port, 55432);
    }

    void environmentOverridesDefaults()
    {
        setValidDatabaseEnvironment();
        qputenv("SERVER_HOST", "0.0.0.0");
        qputenv("SERVER_PORT", "0");
        qputenv("LOG_LEVEL", "ERROR");
        qputenv("CORS_ALLOWED_ORIGIN", "https://cppforge.example");
        qputenv("PG_PORT", "55433");

        const auto config = cppforge::server::ServerConfig::fromEnvironment();

        QCOMPARE(QString::fromStdString(config.host), QStringLiteral("0.0.0.0"));
        QCOMPARE(config.port, std::uint16_t(0));
        QCOMPARE(QString::fromStdString(config.logLevel), QStringLiteral("error"));
        QCOMPARE(QString::fromStdString(config.allowedOrigin), QStringLiteral("https://cppforge.example"));
        QCOMPARE(config.database.port, 55433);
    }

    void rejectsInvalidPostgresPort_data()
    {
        QTest::addColumn<QString>("value");
        QTest::newRow("zero") << QStringLiteral("0");
        QTest::newRow("negative") << QStringLiteral("-1");
        QTest::newRow("too-large") << QStringLiteral("65536");
        QTest::newRow("not-a-number") << QStringLiteral("abc");
    }

    void rejectsInvalidPostgresPort()
    {
        QFETCH(QString, value);
        setValidDatabaseEnvironment();
        qputenv("PG_PORT", value.toUtf8());

        const auto error = configurationError();
        QVERIFY(error.has_value());
        QVERIFY(QString::fromStdString(*error).contains(QStringLiteral("PG_PORT")));
    }

    void acceptsEphemeralAndMaximumServerPorts_data()
    {
        QTest::addColumn<QString>("value");
        QTest::addColumn<std::uint16_t>("expected");
        QTest::newRow("ephemeral") << QStringLiteral("0") << std::uint16_t(0);
        QTest::newRow("maximum") << QStringLiteral("65535") << std::uint16_t(65535);
    }

    void acceptsEphemeralAndMaximumServerPorts()
    {
        QFETCH(QString, value);
        QFETCH(std::uint16_t, expected);
        setValidDatabaseEnvironment();
        qputenv("SERVER_PORT", value.toUtf8());

        const auto config = cppforge::server::ServerConfig::fromEnvironment();
        QCOMPARE(config.port, expected);
    }

    void rejectsInvalidServerPort_data()
    {
        QTest::addColumn<QString>("value");
        QTest::newRow("negative") << QStringLiteral("-1");
        QTest::newRow("too-large") << QStringLiteral("65536");
        QTest::newRow("not-a-number") << QStringLiteral("abc");
    }

    void rejectsInvalidServerPort()
    {
        QFETCH(QString, value);
        setValidDatabaseEnvironment();
        qputenv("SERVER_PORT", value.toUtf8());

        const auto error = configurationError();
        QVERIFY(error.has_value());
        QVERIFY(QString::fromStdString(*error).contains(QStringLiteral("SERVER_PORT")));
    }

    void reportsMissingRequiredVariables_data()
    {
        QTest::addColumn<QString>("name");
        QTest::newRow("host") << QStringLiteral("PG_HOST");
        QTest::newRow("port") << QStringLiteral("PG_PORT");
        QTest::newRow("database") << QStringLiteral("PG_DB");
        QTest::newRow("user") << QStringLiteral("PG_USER");
    }

    void reportsMissingRequiredVariables()
    {
        QFETCH(QString, name);
        setValidDatabaseEnvironment();
        qunsetenv(name.toUtf8().constData());

        const auto error = configurationError();
        QVERIFY(error.has_value());
        QVERIFY2(QString::fromStdString(*error).contains(name), qPrintable(QString::fromStdString(*error)));
    }
};

QTEST_GUILESS_MAIN(TestServerConfig)

#include "test_ServerConfig.moc"
