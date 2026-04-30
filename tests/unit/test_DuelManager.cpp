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

        client.joinRoom("127.0.0.1", 4242);

        QVERIFY(hostConnectedSpy.wait(2000));
        QVERIFY(clientConnectedSpy.wait(2000));

        QCOMPARE(host.getOpponentName(), QString("ClientPlayer"));
        QCOMPARE(client.getOpponentName(), QString("HostPlayer"));

        QSignalSpy taskReceivedSpy(&client, &DuelManager::taskReceived);

        CodingTask dummyTask(1, 1, "Test Task", "Desc", "int main() {}", {}, 2000, 256, std::nullopt, std::nullopt);
        host.sendTask(dummyTask);

        QVERIFY(taskReceivedSpy.wait(1000));
        QCOMPARE(taskReceivedSpy.count(), 1);

        CodingTask receivedTask = taskReceivedSpy.at(0).at(0).value<CodingTask>();
        QCOMPARE(receivedTask.getTitle(), QString("Test Task"));

        QSignalSpy progressSpy(&host, &DuelManager::opponentProgressUpdated);
        client.sendProgress({3, 5});

        QVERIFY(progressSpy.wait(1000));
        auto progress = progressSpy.at(0).at(0).value<DuelProgress>();
        QCOMPARE(progress.passedTests, 3u);

        QSignalSpy winSpy(&host, &DuelManager::duelLost);
        client.sendWin();

        QVERIFY(winSpy.wait(1000));
        QCOMPARE(winSpy.count(), 1);

        client.disconnectAll();
        host.disconnectAll();
    }
};

QTEST_GUILESS_MAIN(TestDuelManager)
#include "test_DuelManager.moc"