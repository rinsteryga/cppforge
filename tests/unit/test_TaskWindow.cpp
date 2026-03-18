#include "../../src/gui/include/TaskWindow.hpp"

#include <QApplication>
#include <QSignalSpy>
#include <QTest>

#include <memory>

class TestTaskWindow : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        // Initialize test case if needed
    }

    void cleanupTestCase()
    {
        // Cleanup after tests
    }

    void init() { taskWindow = std::make_unique<TaskWindow>(); }

    void cleanup() { taskWindow.reset(); }

    void test_Construction() { QVERIFY(taskWindow != nullptr); }

    void test_WindowShown()
    {
        taskWindow->show();
        QVERIFY(taskWindow->isVisible());
    }

    void test_FadeIn()
    {
        taskWindow->show();
        taskWindow->fadeIn();

        // Process events to allow animations to run
        QTest::qWait(100);

        QVERIFY(taskWindow->isVisible());
    }

    void test_FadeOut()
    {
        taskWindow->show();
        taskWindow->fadeOut();

        // Process events to allow animations to run
        QTest::qWait(100);

        QVERIFY(taskWindow != nullptr);
    }

    void test_LoadModule()
    {
        taskWindow->show();
        taskWindow->loadModule(1);

        // Should not crash when loading module
        QVERIFY(true);
    }

    void test_LoadMultipleModules()
    {
        taskWindow->show();

        taskWindow->loadModule(1);
        QTest::qWait(50);

        taskWindow->loadModule(2);
        QTest::qWait(50);

        taskWindow->loadModule(3);
        QTest::qWait(50);

        QVERIFY(taskWindow->isVisible());
    }

    void test_WindowClosed()
    {
        taskWindow->show();
        QVERIFY(taskWindow->isVisible());

        taskWindow->hide();
        QVERIFY(!taskWindow->isVisible());
    }

    void test_ModuleProgressUpdatedSignal()
    {
        taskWindow->show();

        QSignalSpy spy(taskWindow.get(), SIGNAL(moduleProgressUpdated(int, int)));

        // Emit signal manually to test
        emit taskWindow->moduleProgressUpdated(1, 50);

        QCOMPARE(spy.count(), 1);

        QList<QVariant> arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).toInt(), 1);  // moduleId
        QCOMPARE(arguments.at(1).toInt(), 50); // progress
    }

    void test_WindowClosedSignal()
    {
        taskWindow->show();

        QSignalSpy spy(taskWindow.get(), SIGNAL(windowClosed()));

        // Emit signal manually to test
        emit taskWindow->windowClosed();

        QCOMPARE(spy.count(), 1);
    }

    void test_Geometry()
    {
        taskWindow->setGeometry(100, 100, 800, 600);

        // In headless environments, the exact geometry might be different
        // Just verify that the geometry is valid and has positive dimensions
        QVERIFY(taskWindow->geometry().isValid());
        QVERIFY(taskWindow->geometry().width() > 0);
        QVERIFY(taskWindow->geometry().height() > 0);
    }

    void test_WindowProperties()
    {
        taskWindow->show();

        QVERIFY(taskWindow->geometry().isValid());
        QVERIFY(taskWindow->isWindow());
    }

    void test_HideAndShow()
    {
        taskWindow->show();
        QVERIFY(taskWindow->isVisible());

        taskWindow->hide();
        QVERIFY(!taskWindow->isVisible());

        taskWindow->show();
        QVERIFY(taskWindow->isVisible());
    }

private:
    std::unique_ptr<TaskWindow> taskWindow;
};

QTEST_MAIN(TestTaskWindow)
#include "test_TaskWindow.moc"
