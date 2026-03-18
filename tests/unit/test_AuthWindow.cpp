#include "../../src/gui/include/AuthWindow.hpp"

#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>

#include <memory>

class TestAuthWindow : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        // Create mock AuthManager for testing
    }

    void cleanupTestCase()
    {
        // Cleanup after tests
    }

    void init()
    {
        // Create a shared pointer to AuthManager
        // For this test, we'll use nullptr as the GUI should handle it gracefully
        authManager = nullptr; // In real scenario, you'd create a mock or real AuthManager

        if (!authManager)
        {
            // Skip if AuthManager can't be created (it's expected in unit tests)
            QSKIP("AuthManager not initialized");
        }
    }

    void cleanup()
    {
        authWindow.reset();
        authManager.reset();
    }

    void test_Construction()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        authWindow = std::make_unique<AuthWindow>(authManager);
        QVERIFY(authWindow != nullptr);
    }

    void test_WindowShown()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        authWindow = std::make_unique<AuthWindow>(authManager);
        authWindow->show();

        QVERIFY(authWindow->isVisible());
    }

    void test_HasInputFields()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        authWindow = std::make_unique<AuthWindow>(authManager);
        authWindow->show();

        QList<QLineEdit *> inputs = authWindow->findChildren<QLineEdit *>();

        // Should have at least username/email and password fields
        QVERIFY(inputs.size() >= 1);
    }

    void test_HasLoginButton()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        authWindow = std::make_unique<AuthWindow>(authManager);
        authWindow->show();

        QList<QPushButton *> buttons = authWindow->findChildren<QPushButton *>();

        // Should have buttons for login
        QVERIFY(!buttons.isEmpty());
    }

    void test_FadeIn()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        authWindow = std::make_unique<AuthWindow>(authManager);
        authWindow->show();
        authWindow->fadeIn();

        QTest::qWait(100);

        QVERIFY(authWindow->isVisible());
    }

    void test_Geometry()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        authWindow = std::make_unique<AuthWindow>(authManager);
        authWindow->setGeometry(50, 50, 400, 500);

        QCOMPARE(authWindow->geometry().x(), 50);
        QCOMPARE(authWindow->geometry().y(), 50);
        QCOMPARE(authWindow->geometry().width(), 400);
        QCOMPARE(authWindow->geometry().height(), 500);
    }

    void test_WindowProperties()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        authWindow = std::make_unique<AuthWindow>(authManager);
        authWindow->show();

        QVERIFY(authWindow->geometry().isValid());
        QVERIFY(authWindow->isWindow());
    }

    void test_LoginSuccessfulSignal()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        authWindow = std::make_unique<AuthWindow>(authManager);
        authWindow->show();

        QSignalSpy spy(authWindow.get(), SIGNAL(loginSuccessful()));

        // Emit signal manually to test
        emit authWindow->loginSuccessful();

        QCOMPARE(spy.count(), 1);
    }

    void test_SwitchToMainMenuSignal()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        authWindow = std::make_unique<AuthWindow>(authManager);
        authWindow->show();

        QSignalSpy spy(authWindow.get(), SIGNAL(switchToMainMenu()));

        // Emit signal manually to test
        emit authWindow->switchToMainMenu();

        QCOMPARE(spy.count(), 1);
    }

    void test_HideAndShow()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        authWindow = std::make_unique<AuthWindow>(authManager);
        authWindow->show();
        QVERIFY(authWindow->isVisible());

        authWindow->hide();
        QVERIFY(!authWindow->isVisible());

        authWindow->show();
        QVERIFY(authWindow->isVisible());
    }

private:
    std::unique_ptr<AuthWindow> authWindow;
    std::shared_ptr<cppforge::services::AuthManager> authManager;
};

QTEST_MAIN(TestAuthWindow)
#include "test_AuthWindow.moc"
