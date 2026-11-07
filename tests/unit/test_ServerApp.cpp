#include <server/ServerApp.hpp>

#include <QTcpServer>
#include <QtTest>

#include <stdexcept>

namespace
{
    cppforge::server::ServerConfig serverConfig(std::uint16_t port)
    {
        cppforge::server::ServerConfig config;
        config.host = "127.0.0.1";
        config.port = port;
        config.logLevel = "error";
        config.database = {.host = "127.0.0.1",
                           .port = 1,
                           .database = "cppforge_unused",
                           .user = "cppforge_unused",
                           .password = "unused"};
        return config;
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
};

QTEST_GUILESS_MAIN(TestServerApp)

#include "test_ServerApp.moc"
