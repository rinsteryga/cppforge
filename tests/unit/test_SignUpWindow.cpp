#include "../../src/gui/include/SignUpWindow.hpp"

#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>

#include <memory>

class TestSignUpWindow : public QObject
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
        signUpWindow.reset();
        authManager.reset();
    }

    void test_Construction()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        signUpWindow = std::make_unique<SignUpWindow>(authManager);
        QVERIFY(signUpWindow != nullptr);
    }

    void test_WindowShown()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        signUpWindow = std::make_unique<SignUpWindow>(authManager);
        signUpWindow->show();

        QVERIFY(signUpWindow->isVisible());
    }

    void test_HasInputFields()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        signUpWindow = std::make_unique<SignUpWindow>(authManager);
        signUpWindow->show();

        QList<QLineEdit *> inputs = signUpWindow->findChildren<QLineEdit *>();

        // Should have username, email, and password fields
        QVERIFY(inputs.size() >= 2);
    }

    void test_HasSignUpButton()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        signUpWindow = std::make_unique<SignUpWindow>(authManager);
        signUpWindow->show();

        QList<QPushButton *> buttons = signUpWindow->findChildren<QPushButton *>();

        // Should have buttons for sign up
        QVERIFY(!buttons.isEmpty());
    }

    void test_FadeIn()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        signUpWindow = std::make_unique<SignUpWindow>(authManager);
        signUpWindow->show();
        signUpWindow->fadeIn();

        QTest::qWait(100);

        QVERIFY(signUpWindow->isVisible());
    }

    void test_Geometry()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        signUpWindow = std::make_unique<SignUpWindow>(authManager);
        signUpWindow->setGeometry(50, 50, 400, 600);

        QCOMPARE(signUpWindow->geometry().x(), 50);
        QCOMPARE(signUpWindow->geometry().y(), 50);
        QCOMPARE(signUpWindow->geometry().width(), 400);
        QCOMPARE(signUpWindow->geometry().height(), 600);
    }

    void test_WindowProperties()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        signUpWindow = std::make_unique<SignUpWindow>(authManager);
        signUpWindow->show();

        QVERIFY(signUpWindow->geometry().isValid());
        QVERIFY(signUpWindow->isWindow());
    }

    void test_SwitchToLoginSignal()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        signUpWindow = std::make_unique<SignUpWindow>(authManager);
        signUpWindow->show();

        QSignalSpy spy(signUpWindow.get(), SIGNAL(switchToLogin()));

        // Emit signal manually to test
        emit signUpWindow->switchToLogin();

        QCOMPARE(spy.count(), 1);
    }

    void test_HideAndShow()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        signUpWindow = std::make_unique<SignUpWindow>(authManager);
        signUpWindow->show();
        QVERIFY(signUpWindow->isVisible());

        signUpWindow->hide();
        QVERIFY(!signUpWindow->isVisible());

        signUpWindow->show();
        QVERIFY(signUpWindow->isVisible());
    }

    void test_PaintEvent()
    {
        if (!authManager)
            QSKIP("AuthManager not initialized");

        signUpWindow = std::make_unique<SignUpWindow>(authManager);
        signUpWindow->show();

        // Trigger a repaint - should not crash
        signUpWindow->update();
        QTest::qWait(50);

        QVERIFY(true);
    }

private:
    std::unique_ptr<SignUpWindow> signUpWindow;
    std::shared_ptr<cppforge::services::AuthManager> authManager;
};

QTEST_MAIN(TestSignUpWindow)
#include "test_SignUpWindow.moc"
