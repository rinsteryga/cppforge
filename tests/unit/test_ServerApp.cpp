#include <server/ServerApp.hpp>

#include <QEventLoop>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTcpServer>
#include <QTimer>
#include <QUrl>
#include <QtTest>

#include <sstream>
#include <stdexcept>

namespace
{
    cppforge::server::ServerConfig serverConfig(std::uint16_t port, const std::string &logLevel = "error")
    {
        cppforge::server::ServerConfig config;
        config.host = "127.0.0.1";
        config.port = port;
        config.logLevel = logLevel;
        config.database = {.host = "127.0.0.1",
                           .port = 1,
                           .database = "cppforge_unused",
                           .user = "cppforge_unused",
                           .password = "unused"};
        return config;
    }

    int getStatus(const QUrl &url)
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

        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        reply->deleteLater();
        return status;
    }
} // namespace

class TestServerApp : public QObject
{
    Q_OBJECT

private slots:
    void bindFailureCanBeRetried()
    {
        QTcpServer blocker;
        QVERIFY(blocker.listen(QHostAddress::LocalHost, 0));
        const std::uint16_t occupiedPort = blocker.serverPort();

        cppforge::server::ServerApp server(serverConfig(occupiedPort));
        QVERIFY_EXCEPTION_THROWN(server.start(), std::runtime_error);
        server.stop();

        blocker.close();
        server.start();
        QVERIFY(server.port() > 0);
        server.stop();
    }

    void accessLogRespectsConfiguredLevel()
    {
        for (const auto &logLevel : {std::string("error"), std::string("info")})
        {
            cppforge::server::ServerApp server(serverConfig(0, logLevel));
            server.start();
            const QUrl healthUrl(QStringLiteral("http://127.0.0.1:%1/api/v1/health").arg(server.port()));

            std::ostringstream captured;
            auto *previous = std::clog.rdbuf(captured.rdbuf());
            QCOMPARE(getStatus(healthUrl), 200);
            server.stop();
            std::clog.rdbuf(previous);

            const bool hasAccessLog = captured.str().find("request_id=") != std::string::npos;
            QCOMPARE(hasAccessLog, logLevel == "info");
        }
    }
};

QTEST_GUILESS_MAIN(TestServerApp)

#include "test_ServerApp.moc"
