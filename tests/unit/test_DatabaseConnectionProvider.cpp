#include <data/DatabaseConnectionProvider.hpp>
#include <data/DatabaseErrors.hpp>

#include <QElapsedTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QTcpServer>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtTest>

#include <atomic>
#include <chrono>
#include <mutex>
#include <optional>
#include <set>
#include <thread>
#include <vector>

namespace
{
    QMutex g_messageMutex;
    QStringList g_messages;
    QtMessageHandler g_previousHandler = nullptr;

    void captureMessages(QtMsgType type, const QMessageLogContext &context, const QString &message)
    {
        {
            QMutexLocker locker(&g_messageMutex);
            g_messages.append(message);
        }
        if (g_previousHandler != nullptr)
        {
            g_previousHandler(type, context, message);
        }
    }

    void startMessageCapture()
    {
        QMutexLocker locker(&g_messageMutex);
        g_messages.clear();
        g_previousHandler = qInstallMessageHandler(captureMessages);
    }

    QStringList stopMessageCapture()
    {
        qInstallMessageHandler(g_previousHandler);
        QMutexLocker locker(&g_messageMutex);
        return g_messages;
    }

    cppforge::data::DatabaseConfig databaseConfig(const QString &host, int port, const QString &database,
                                                  const QString &user, const QString &password)
    {
        return {.host = host.toStdString(),
                .port = port,
                .database = database.toStdString(),
                .user = user.toStdString(),
                .password = password.toStdString()};
    }

    std::optional<cppforge::data::DatabaseConfig> testDatabaseConfig()
    {
        const QString host = qEnvironmentVariable("TEST_PG_HOST");
        const QString user = qEnvironmentVariable("TEST_PG_USER");
        if (host.isEmpty() || user.isEmpty())
        {
            return std::nullopt;
        }

        bool validPort = false;
        const int port = qEnvironmentVariable("TEST_PG_PORT", "5432").toInt(&validPort);
        if (!validPort || port < 1 || port > 65535)
        {
            return std::nullopt;
        }

        return databaseConfig(host, port, qEnvironmentVariable("TEST_PG_DB", "cppforge_test"), user,
                              qEnvironmentVariable("TEST_PG_PASSWORD"));
    }

    int closedLocalPort()
    {
        QTcpServer server;
        if (!server.listen(QHostAddress::LocalHost, 0))
        {
            return 0;
        }
        const int port = server.serverPort();
        server.close();
        return port;
    }
} // namespace

class TestDatabaseConnectionProvider : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QPSQL")))
        {
            QSKIP("QPSQL driver is not available");
        }
    }

    void unreachableDatabaseFailsWithDatabaseUnavailable()
    {
        const int port = closedLocalPort();
        QVERIFY(port > 0);

        const cppforge::data::DatabaseConnectionProvider provider(
            databaseConfig(QStringLiteral("127.0.0.1"), port, QStringLiteral("cppforge_test"),
                           QStringLiteral("cppforge"), QStringLiteral("invalid")));

        QElapsedTimer timer;
        timer.start();
        QVERIFY(!provider.ping());
        QVERIFY2(timer.elapsed() < 6000, "Connection attempt did not honor the connect timeout");

        QVERIFY_EXCEPTION_THROWN(provider.ensureHealthy(), cppforge::data::DatabaseUnavailable);
    }

    void connectionReopensAfterBeingClosed()
    {
        const auto config = testDatabaseConfig();
        if (!config)
        {
            QSKIP("Set TEST_PG_* variables to run the database provider lifecycle test");
        }

        const cppforge::data::DatabaseConnectionProvider provider(*config);
        {
            auto database = provider.connectionForCurrentThread();
            QVERIFY(database.isOpen());
            database.close();
        }
        QVERIFY(provider.ping());
    }

    void providerCanBeDestroyedInAWorkerThread()
    {
        const auto config = testDatabaseConfig();
        if (!config)
        {
            QSKIP("Set TEST_PG_* variables to run the database provider lifecycle test");
        }

        std::atomic<bool> succeeded{false};
        startMessageCapture();
        std::thread worker(
            [&config, &succeeded]
            {
                cppforge::data::DatabaseConnectionProvider provider(*config);
                auto database = provider.connectionForCurrentThread();
                QSqlQuery query(database);
                succeeded = query.exec(QStringLiteral("SELECT 1")) && query.next();
            });
        worker.join();
        const QStringList messages = stopMessageCapture();

        QVERIFY(succeeded.load());
        for (const auto &message : messages)
        {
            QVERIFY2(!message.contains(QStringLiteral("still in use")),
                     qPrintable(QStringLiteral("Unexpected Qt SQL warning: %1").arg(message)));
        }
    }

    void connectionsAreThreadLocal()
    {
        const auto config = testDatabaseConfig();
        if (!config)
        {
            QSKIP("Set TEST_PG_* variables to run the database provider lifecycle test");
        }

        const cppforge::data::DatabaseConnectionProvider provider(*config);
        std::mutex mutex;
        std::set<int> backendPids;
        std::atomic<int> failures{0};

        std::vector<std::thread> workers;
        for (int index = 0; index < 3; ++index)
        {
            workers.emplace_back(
                [&provider, &mutex, &backendPids, &failures]
                {
                    try
                    {
                        auto database = provider.connectionForCurrentThread();
                        QSqlQuery query(database);
                        if (query.exec(QStringLiteral("SELECT pg_backend_pid()")) && query.next())
                        {
                            std::lock_guard<std::mutex> lock(mutex);
                            backendPids.insert(query.value(0).toInt());
                        }
                        else
                        {
                            ++failures;
                        }
                    }
                    catch (const std::exception &)
                    {
                        ++failures;
                    }
                });
        }
        for (auto &worker : workers)
        {
            worker.join();
        }

        QCOMPARE(failures.load(), 0);
        QCOMPARE(backendPids.size(), std::size_t(3));
    }
};

QTEST_GUILESS_MAIN(TestDatabaseConnectionProvider)

#include "test_DatabaseConnectionProvider.moc"
