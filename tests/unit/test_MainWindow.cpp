#include "MainWindow.hpp"

#include <QtTest>
#include <QTest>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <memory>

class TestMainWindow : public QObject
{
    Q_OBJECT

private slots:
    void init()
    {
        mainWindow = std::make_unique<MainWindow>();
    }

    void cleanup()
    {
        if (mainWindow) {
            mainWindow.reset();
        }
    }

    // Test window initialization
    void test_WindowCreation()
    {
        QVERIFY(mainWindow != nullptr);
        QCOMPARE(mainWindow->size(), QSize(1280, 900));
    }

    void test_WindowTitle()
    {
        QCOMPARE(mainWindow->windowTitle(), "cppforge - Main Menu");
    }

private:
    std::unique_ptr<MainWindow> mainWindow;
};

QTEST_MAIN(TestMainWindow)
#include "test_MainWindow.moc"
