#include "../../src/core/include/entities/CodingTask.hpp"
#include "../../src/core/include/services/DuelManager.hpp"

#include <QSignalSpy>
#include <QtTest/QtTest>

using namespace cppforge::services;
using namespace cppforge::entities;

class TestDuelManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}

    void testConnectionAndIdentityExchange()
    {
        DuelManager host("HostPlayer");
        DuelManager client("ClientPlayer");

        QSignalSpy hostConnectedSpy(&host, &DuelManager::opponentConnected);
        QSignalSpy clientConnectedSpy(&client, &DuelManager::opponentConnected);
        QSignalSpy hostIdentifiedSpy(&host, &DuelManager::opponentIdentified);
        QSignalSpy clientIdentifiedSpy(&client, &DuelManager::opponentIdentified);

        QVERIFY(host.hostRoom(4242));

        client.joinRoom("127.0.0.1", 4242);

        if (hostConnectedSpy.isEmpty())
            hostConnectedSpy.wait(5000);
        if (clientConnectedSpy.isEmpty())
            clientConnectedSpy.wait(5000);

        QVERIFY2(!hostConnectedSpy.isEmpty(), "Host timed out waiting for connection");
        QVERIFY2(!clientConnectedSpy.isEmpty(), "Client timed out waiting for connection");

        if (hostIdentifiedSpy.isEmpty())
            hostIdentifiedSpy.wait(2000);
        if (clientIdentifiedSpy.isEmpty())
            clientIdentifiedSpy.wait(2000);

        QVERIFY2(!hostIdentifiedSpy.isEmpty(), "Host never identified opponent");
        QVERIFY2(!clientIdentifiedSpy.isEmpty(), "Client never identified opponent");

        QCOMPARE(host.getOpponentName(), QString("ClientPlayer"));
        QCOMPARE(client.getOpponentName(), QString("HostPlayer"));
    }
};

QTEST_GUILESS_MAIN(TestDuelManager)
#include "test_DuelManager.moc"