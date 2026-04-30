#include "../../src/core/include/entities/CodingTask.hpp"
#include "../../src/core/include/services/DuelManager.hpp"

#include <QSignalSpy>
#include <QTimer>
#include <QtTest/QtTest>

using namespace cppforge::services;
using namespace cppforge::entities;

class TestDuelManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qRegisterMetaType<CodingTask>("CodingTask");
        qRegisterMetaType<DuelProgress>("DuelProgress");
    }

    void testConnectionAndIdentityExchange()
    {
        DuelManager host("HostPlayer");
        DuelManager client("ClientPlayer");

        QSignalSpy hostConnectedSpy(&host, &DuelManager::opponentConnected);
        QSignalSpy clientConnectedSpy(&client, &DuelManager::opponentConnected);

        QVERIFY(host.hostRoom(4242));

        QTest::qWait(500);

        client.joinRoom("127.0.0.1", 4242);

        int timeout = 10000;
        while (timeout > 0 && (hostConnectedSpy.isEmpty() || clientConnectedSpy.isEmpty()))
        {
            QTest::qWait(100);
            timeout -= 100;
        }

        QVERIFY2(!hostConnectedSpy.isEmpty(), "Host timed out");
        QVERIFY2(!clientConnectedSpy.isEmpty(), "Client timed out");

        QCOMPARE(hostConnectedSpy.count(), 1);
        QCOMPARE(clientConnectedSpy.count(), 1);
    }
};

QTEST_GUILESS_MAIN(TestDuelManager)
#include "test_DuelManager.moc"