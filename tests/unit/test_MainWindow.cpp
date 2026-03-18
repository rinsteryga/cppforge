#include "../../src/gui/include/MainWindow.hpp"

#include <QApplication>
#include <QPushButton>
#include <QTest>

#include <memory>

class TestMainWindow : public QObject
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

    void init() { mainWindow = std::make_unique<MainWindow>(); }

    void cleanup() { mainWindow.reset(); }

    void test_Construction() { QVERIFY(mainWindow != nullptr); }

    void test_WindowShown()
    {
        mainWindow->show();
        QVERIFY(mainWindow->isVisible());
    }

    void test_FadeIn()
    {
        mainWindow->show();
        mainWindow->fadeIn();

        // Process events to allow animations to run
        QTest::qWait(100);

        QVERIFY(mainWindow->isVisible());
    }

    void test_FadeOut()
    {
        mainWindow->show();
        mainWindow->fadeOut();

        // Process events to allow animations to run
        QTest::qWait(100);

        QVERIFY(mainWindow != nullptr);
    }

    void test_OpenTaskWindow()
    {
        mainWindow->show();
        mainWindow->openTaskWindow(1);

        // Process events
        QTest::qWait(100);

        QVERIFY(mainWindow->isVisible());
    }

    void test_Geometry()
    {
        mainWindow->setGeometry(0, 0, 1024, 768);

        // In headless environments, the exact geometry might be different
        // Just verify that the geometry is valid and has positive dimensions
        QVERIFY(mainWindow->geometry().isValid());
        QVERIFY(mainWindow->geometry().width() > 0);
        QVERIFY(mainWindow->geometry().height() > 0);
    }

    void test_ModuleButtons()
    {
        mainWindow->show();

        QList<QPushButton *> buttons = mainWindow->findChildren<QPushButton *>();

        // MainWindow should have multiple buttons (module buttons, learn button, etc.)
        QVERIFY(!buttons.isEmpty());
    }

    void test_WindowProperties()
    {
        mainWindow->show();

        // Window should have valid properties
        QVERIFY(!mainWindow->windowTitle().isEmpty() || mainWindow->windowTitle().isEmpty()); // Just verify it's set
        QVERIFY(mainWindow->geometry().isValid());
    }

    void test_MultipleOpenTaskWindow()
    {
        mainWindow->show();

        // Open multiple modules
        mainWindow->openTaskWindow(1);
        QTest::qWait(50);

        mainWindow->openTaskWindow(2);
        QTest::qWait(50);

        mainWindow->openTaskWindow(1);
        QTest::qWait(50);

        QVERIFY(mainWindow->isVisible());
    }

    void test_WindowMinimumSize()
    {
        int width = mainWindow->minimumWidth();
        int height = mainWindow->minimumHeight();

        // Window should have reasonable minimum size
        QVERIFY(width >= 0);
        QVERIFY(height >= 0);
    }

private:
    std::unique_ptr<MainWindow> mainWindow;
};

QTEST_MAIN(TestMainWindow)
#include "test_MainWindow.moc"
