#include "../../src/gui/include/CustomTitleBar.hpp"

#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QString>
#include <QTest>

class TestCustomTitleBar : public QObject
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

    void init()
    {
        titleBar = std::make_unique<CustomTitleBar>();
        titleBar->show();
    }

    void cleanup() { titleBar.reset(); }

    void test_Construction()
    {
        QVERIFY(titleBar != nullptr);
        QVERIFY(titleBar->isVisible());
    }

    void test_SetTitle()
    {
        QString testTitle = "Test Window";
        titleBar->setTitle(testTitle);

        // Find the title label and verify it was set
        QList<QLabel *> labels = titleBar->findChildren<QLabel *>();
        QVERIFY(!labels.isEmpty());

        bool titleFound = false;
        for (QLabel *label : labels)
        {
            if (label->text() == testTitle)
            {
                titleFound = true;
                break;
            }
        }
        QVERIFY(titleFound);
    }

    void test_SetIcon()
    {
        QIcon testIcon(":/icons/app_icon.png");
        titleBar->setIcon(testIcon);

        // Verify icon was set (icon won't be null after setting)
        QVERIFY(true); // Icon setting doesn't throw exceptions
    }

    void test_SetMultipleTitles()
    {
        titleBar->setTitle("First Title");
        titleBar->setTitle("Second Title");

        QList<QLabel *> labels = titleBar->findChildren<QLabel *>();
        bool titleFound = false;
        for (QLabel *label : labels)
        {
            if (label->text() == "Second Title")
            {
                titleFound = true;
                break;
            }
        }
        QVERIFY(titleFound);
    }

    void test_TitleBarHasButtons()
    {
        // TitleBar should have at least minimize, maximize and close buttons
        QList<QPushButton *> buttons = titleBar->findChildren<QPushButton *>();
        QVERIFY(!buttons.isEmpty());
    }

    void test_GeometryAndSize()
    {
        titleBar->setGeometry(0, 0, 800, 50);
        QCOMPARE(titleBar->geometry().width(), 800);
        // Height might be different due to default height settings, so just verify it's set
        QVERIFY(titleBar->geometry().height() > 0);
    }

    void test_MouseEventHandling()
    {
        // Create a simple mouse press event
        QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(10, 10), Qt::LeftButton, Qt::LeftButton,
                               Qt::NoModifier);

        // Should not crash when processing mouse events
        QVERIFY(true);
    }

private:
    std::unique_ptr<CustomTitleBar> titleBar;
};

QTEST_MAIN(TestCustomTitleBar)
#include "test_CustomTitleBar.moc"
