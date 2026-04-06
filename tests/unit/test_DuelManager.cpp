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
    void initTestCase() {}

    void testConnectionAndMessaging()
    {
        DuelManager host;
        DuelManager client;

        QSignalSpy hostConnectedSpy(&host, &DuelManager::opponentConnected);

        QVERIFY(host.hostRoom(4242));

        client.joinRoom("127.0.0.1", 4242);

        QVERIFY(hostConnectedSpy.wait(1000));
        QCOMPARE(hostConnectedSpy.count(), 1);

        QSignalSpy taskReceivedSpy(&client, &DuelManager::taskReceived);

        CodingTask dummyTask(1, 1, "Test Task", "Desc", "int main() {}", {}, 2000, 256, std::nullopt, std::nullopt);
        host.sendTask(dummyTask);

        QVERIFY(taskReceivedSpy.wait(1000));
        QCOMPARE(taskReceivedSpy.count(), 1);

        QList<QVariant> taskArgs = taskReceivedSpy.takeFirst();
        CodingTask receivedTask = taskArgs.at(0).value<CodingTask>();
        QCOMPARE(receivedTask.getId(), static_cast<uint64_t>(1));
        QCOMPARE(receivedTask.getTitle(), QString("Test Task"));

        QSignalSpy progressSpy(&host, &DuelManager::opponentProgressUpdated);
        client.sendProgress({3, 5});

        QVERIFY(progressSpy.wait(1000));
        QCOMPARE(progressSpy.count(), 1);
        QList<QVariant> progressArgs = progressSpy.takeFirst();
        auto progress = progressArgs.at(0).value<DuelProgress>();
        QCOMPARE(progress.passedTests, static_cast<uint32_t>(3));
        QCOMPARE(progress.totalTests, static_cast<uint32_t>(5));

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
