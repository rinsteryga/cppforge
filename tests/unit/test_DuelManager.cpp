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

    void TestDuelManager::testConnectionAndIdentityExchange()
    {
        // Используем уникальные имена, чтобы проверить передачу ника
        auto host = std::make_unique<DuelManager>("HostPlayer", this);
        auto client = std::make_unique<DuelManager>("ClientPlayer", this);

        QSignalSpy hostConnectedSpy(host.get(), &DuelManager::opponentConnected);
        QSignalSpy clientConnectedSpy(client.get(), &DuelManager::opponentConnected);

        QVERIFY(host->hostRoom(4242));

        QTest::qWait(500);

        client->joinRoom("127.0.0.1", 4242);

        bool hostReceived = hostConnectedSpy.wait(5000);
        bool clientReceived = clientConnectedSpy.wait(5000);

        if (!hostReceived || !clientReceived)
        {
            QFAIL("Connection timeout: Host or Client didn't receive opponentConnected signal in 5s");
        }

        QCOMPARE(hostConnectedSpy.count(), 1);
        QCOMPARE(clientConnectedSpy.count(), 1);
    }
}

QTEST_GUILESS_MAIN(TestDuelManager)
#include "test_DuelManager.moc"