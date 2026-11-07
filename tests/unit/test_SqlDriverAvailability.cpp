#include <QtSql/QSqlDatabase>
#include <QtTest>

class TestSqlDriverAvailability : public QObject
{
    Q_OBJECT

private slots:
    void qpsqlDriverIsAvailable()
    {
        const QStringList drivers = QSqlDatabase::drivers();
        qInfo() << "Available Qt SQL drivers:" << drivers;
        QVERIFY2(drivers.contains(QStringLiteral("QPSQL")), "QPSQL driver is not available");
    }
};

QTEST_GUILESS_MAIN(TestSqlDriverAvailability)

#include "test_SqlDriverAvailability.moc"
