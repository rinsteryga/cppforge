#include <data/DatabaseConnectionProvider.hpp>
#include <nlohmann/json.hpp>
#include <server/ServerApp.hpp>

#include <QEventLoop>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include <QtSql/QSqlQuery>
#include <QtTest>

#include <memory>
#include <stdexcept>
#include <string>

namespace
{
    struct HttpResult
    {
        int status{};
        QByteArray body;
        QByteArray requestId;
    };

    cppforge::data::DatabaseConfig testDatabaseConfig()
    {
        const QString host = qEnvironmentVariable("TEST_PG_HOST");
        const QString portString = qEnvironmentVariable("TEST_PG_PORT", "5432");
        const QString user = qEnvironmentVariable("TEST_PG_USER");
        if (host.isEmpty() || user.isEmpty())
        {
            throw std::runtime_error("TEST_PG_HOST and TEST_PG_USER are required");
        }

        bool validPort = false;
        const int port = portString.toInt(&validPort);
        if (!validPort || port < 1 || port > 65535)
        {
            throw std::runtime_error("Invalid TEST_PG_PORT");
        }

        return {.host = host.toStdString(),
                .port = port,
                .database = qEnvironmentVariable("TEST_PG_DB", "cppforge_test").toStdString(),
                .user = user.toStdString(),
                .password = qEnvironmentVariable("TEST_PG_PASSWORD").toStdString()};
    }

    HttpResult get(const QUrl &url)
    {
        QNetworkAccessManager manager;
        QNetworkRequest request(url);
        QEventLoop loop;
        QTimer timeout;
        timeout.setSingleShot(true);

        QNetworkReply *reply = manager.get(request);
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        QObject::connect(&timeout, &QTimer::timeout, reply, &QNetworkReply::abort);
        timeout.start(5000);
        loop.exec();

        const HttpResult result{.status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(),
                                .body = reply->readAll(),
                                .requestId = reply->rawHeader("X-Request-Id")};
        reply->deleteLater();
        return result;
    }

    std::vector<HttpResult> getConcurrent(const QUrl &url, std::size_t count)
    {
        QNetworkAccessManager manager;
        std::vector<QNetworkReply *> replies;
        replies.reserve(count);
        QEventLoop loop;
        QTimer timeout;
        timeout.setSingleShot(true);
        std::size_t remaining = count;

        for (std::size_t index = 0; index < count; ++index)
        {
            auto *reply = manager.get(QNetworkRequest(url));
            replies.push_back(reply);
            QObject::connect(reply, &QNetworkReply::finished, &loop,
                             [&loop, &remaining]
                             {
                                 if (--remaining == 0)
                                 {
                                     loop.quit();
                                 }
                             });
        }

        QObject::connect(&timeout, &QTimer::timeout, &loop,
                         [&replies, &loop]
                         {
                             for (auto *reply : replies)
                             {
                                 reply->abort();
                             }
                             loop.quit();
                         });
        timeout.start(10000);
        loop.exec();

        std::vector<HttpResult> results;
        results.reserve(replies.size());
        for (auto *reply : replies)
        {
            results.push_back({.status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(),
                               .body = reply->readAll(),
                               .requestId = reply->rawHeader("X-Request-Id")});
            reply->deleteLater();
        }
        return results;
    }

    nlohmann::json parseBody(const HttpResult &result)
    {
        return nlohmann::json::parse(result.body.constData());
    }

    nlohmann::json readFixture(const QString &name)
    {
        const QString path = QStringLiteral(CPPFORGE_SOURCE_DIR) + QStringLiteral("/contracts/fixtures/") + name;
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
        {
            throw std::runtime_error("Unable to read API fixture");
        }
        return nlohmann::json::parse(file.readAll().constData());
    }

    bool canConnectToTestDatabase(const cppforge::data::DatabaseConfig &config)
    {
        try
        {
            cppforge::data::DatabaseConnectionProvider provider(config);
            auto database = provider.connectionForCurrentThread();
            QSqlQuery query(database);
            return query.exec("SELECT 1") && query.next();
        }
        catch (const std::exception &)
        {
            return false;
        }
    }

    bool hasRealSchemaAndSeed(const cppforge::data::DatabaseConfig &config)
    {
        cppforge::data::DatabaseConnectionProvider provider(config);
        auto database = provider.connectionForCurrentThread();
        QSqlQuery query(database);
        if (!query.exec("SELECT COUNT(*) FROM information_schema.tables "
                        "WHERE table_schema = 'public' AND table_name IN "
                        "('users', 'modules', 'lessons', 'coding_tasks')") ||
            !query.next() || query.value(0).toInt() != 4)
        {
            return false;
        }

        return query.exec("SELECT COUNT(*) FROM modules") && query.next() && query.value(0).toInt() > 0;
    }
} // namespace

class TestServerIntegration : public QObject
{
    Q_OBJECT

private:
    cppforge::data::DatabaseConfig m_databaseConfig;
    std::unique_ptr<cppforge::server::ServerApp> m_server;
    QUrl m_baseUrl;

private slots: // NOLINT(readability-redundant-access-specifiers)
    void initTestCase()
    {
        try
        {
            m_databaseConfig = testDatabaseConfig();
        }
        catch (const std::exception &)
        {
            QSKIP("Set TEST_PG_* variables and make cppforge_test available to run HTTP integration tests");
        }

        if (!canConnectToTestDatabase(m_databaseConfig))
        {
            QSKIP("cppforge_test is not available");
        }
        QVERIFY(hasRealSchemaAndSeed(m_databaseConfig));

        cppforge::server::ServerConfig config;
        config.database = m_databaseConfig;
        config.port = 0;
        config.host = "127.0.0.1";
        m_server = std::make_unique<cppforge::server::ServerApp>(std::move(config));
        m_server->start();
        m_baseUrl = QUrl(QStringLiteral("http://127.0.0.1:%1").arg(m_server->port()));
    }

    void cleanupTestCase()
    {
        if (m_server)
        {
            m_server->stop();
        }
    }

    void healthAndReadyEndpoints()
    {
        const auto health = get(m_baseUrl.resolved(QUrl(QStringLiteral("/api/v1/health"))));
        QCOMPARE(health.status, 200);
        QVERIFY(parseBody(health) == readFixture(QStringLiteral("health.json")));
        QVERIFY(!health.requestId.isEmpty());

        const auto ready = get(m_baseUrl.resolved(QUrl(QStringLiteral("/api/v1/ready"))));
        QCOMPARE(ready.status, 200);
        QVERIFY(parseBody(ready) == readFixture(QStringLiteral("ready.json")));
    }

    void moduleEndpoints()
    {
        const auto list = get(m_baseUrl.resolved(QUrl(QStringLiteral("/api/v1/modules"))));
        QCOMPARE(list.status, 200);
        const auto payload = parseBody(list);
        QVERIFY(payload.at("meta").at("total").get<std::size_t>() > 0);
        QVERIFY(!payload.at("items").empty());
        QCOMPARE(payload.at("meta").at("total").get<std::size_t>(), payload.at("items").size());

        const auto moduleId = payload.at("items").at(0).at("id").get<std::uint64_t>();
        const auto module = get(m_baseUrl.resolved(QUrl(QStringLiteral("/api/v1/modules/%1").arg(moduleId))));
        QCOMPARE(module.status, 200);
        const auto modulePayload = parseBody(module);
        QCOMPARE(modulePayload.at("id").get<std::uint64_t>(), moduleId);
        QVERIFY(!modulePayload.at("name").get<std::string>().empty());
        QVERIFY(modulePayload.at("description").is_string());
    }

    void notFoundUsesErrorEnvelope()
    {
        const auto missingPath = get(m_baseUrl.resolved(QUrl(QStringLiteral("/api/v1/missing"))));
        QCOMPARE(missingPath.status, 404);
        const auto error = parseBody(missingPath);
        QVERIFY(error.at("error").at("code").get<std::string>() == "NOT_FOUND");
        QVERIFY(error.at("error").contains("message"));
        QVERIFY(error.at("error").contains("details"));

        const auto missingModule = get(m_baseUrl.resolved(QUrl(QStringLiteral("/api/v1/modules/404"))));
        QCOMPARE(missingModule.status, 404);
        QVERIFY(parseBody(missingModule) == readFixture(QStringLiteral("error_not_found.json")));
    }

    void concurrentRequestsUseThreadLocalDatabaseConnections()
    {
        const auto results = getConcurrent(m_baseUrl.resolved(QUrl(QStringLiteral("/api/v1/modules"))), 16);
        QCOMPARE(results.size(), std::size_t(16));
        for (const auto &result : results)
        {
            QCOMPARE(result.status, 200);
            QVERIFY(!result.requestId.isEmpty());
            QVERIFY(parseBody(result).at("meta").at("total").get<std::size_t>() > 0);
        }
    }

    void healthDoesNotRequireDatabase()
    {
        auto config = cppforge::server::ServerConfig{};
        config.database = m_databaseConfig;
        config.database.port = config.database.port == 1 ? 2 : 1;
        config.port = 0;
        config.host = "127.0.0.1";

        cppforge::server::ServerApp offlineServer(std::move(config));
        offlineServer.start();
        const QUrl baseUrl(QStringLiteral("http://127.0.0.1:%1").arg(offlineServer.port()));

        QCOMPARE(get(baseUrl.resolved(QUrl(QStringLiteral("/api/v1/health")))).status, 200);
        const auto ready = get(baseUrl.resolved(QUrl(QStringLiteral("/api/v1/ready"))));
        QCOMPARE(ready.status, 503);
        QVERIFY(parseBody(ready).at("error").at("code").get<std::string>() == "DATABASE_UNAVAILABLE");

        const auto modules = get(baseUrl.resolved(QUrl(QStringLiteral("/api/v1/modules"))));
        QCOMPARE(modules.status, 500);
        const auto error = parseBody(modules);
        QVERIFY(error.at("error").at("code").get<std::string>() == "INTERNAL_ERROR");
        QVERIFY(!modules.body.contains("Database connection unavailable"));
        offlineServer.stop();
    }
};

QTEST_GUILESS_MAIN(TestServerIntegration)

#include "test_ServerIntegration.moc"
