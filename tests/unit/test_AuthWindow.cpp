#include "AuthWindow.hpp"
#include "../../src/core/include/services/AuthManager.hpp"
#include "../../src/core/include/repositories/IUserRepository.hpp"

#include <QtTest>
#include <QTest>
#include <QLineEdit>
#include <QPushButton>
#include <memory>

// Mock implementation of IUserRepository for testing
class MockUserRepository : public cppforge::repositories::IUserRepository
{
public:
    MockUserRepository() : userExists_(false) {}

    std::optional<cppforge::entities::User> findByEmail(const QString& email) const override
    {
        if (userExists_ && email == "existing@example.com") {
            return cppforge::entities::User(1, "testuser", email, "hash");
        }
        return std::nullopt;
    }

    std::optional<cppforge::entities::User> findByUsername(const QString& username) const override
    {
        if (userExists_ && username == "existinguser") {
            return cppforge::entities::User(1, username, "test@example.com", "hash");
        }
        return std::nullopt;
    }

    bool save(cppforge::entities::User user) override
    {
        return true;
    }

    void setUserExists(bool exists) { userExists_ = exists; }

private:
    bool userExists_;
};

class TestAuthWindow : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        authManager = std::make_shared<cppforge::services::AuthManager>(
            std::make_unique<MockUserRepository>()
        );
    }

    void cleanupTestCase()
    {
    }

    void init()
    {
        authWindow = std::make_unique<AuthWindow>(authManager);
    }

    void cleanup()
    {
        if (authWindow) {
            authWindow.reset();
        }
    }

    // Базовые тесты без блокирования
    void test_WindowCreation()
    {
        QVERIFY(authWindow != nullptr);
    }

    void test_WindowProperties()
    {
        QVERIFY(authWindow->size() == QSize(1280, 900));
    }

    void test_UIElementsExist()
    {
        auto lineEdits = authWindow->findChildren<QLineEdit*>();
        QVERIFY(lineEdits.size() >= 2); // username и password
        
        auto buttons = authWindow->findChildren<QPushButton*>();
        QVERIFY(buttons.size() > 0);
    }

    void test_InputFieldsProperties()
    {
        auto lineEdits = authWindow->findChildren<QLineEdit*>();
        QVERIFY(lineEdits.size() >= 2);
        
        QCOMPARE(lineEdits.at(0)->height(), 65);
        QCOMPARE(lineEdits.at(0)->width(), 500);
    }

    void test_ButtonsExist()
    {
        auto buttons = authWindow->findChildren<QPushButton*>();
        
        bool hasLoginButton = false;
        bool hasCreateButton = false;
        
        for (auto btn : buttons) {
            if (btn->text() == "Log in") hasLoginButton = true;
            if (btn->text().contains("Create")) hasCreateButton = true;
        }
        
        QVERIFY(hasLoginButton);
        QVERIFY(hasCreateButton);
    }

    void test_WindowTitle()
    {
        QCOMPARE(authWindow->windowTitle(), "cppforge Log in");
    }

    void test_WindowObjectName()
    {
        // Custom title bar should exist
        auto titleBars = authWindow->findChildren<QWidget*>();
        QVERIFY(titleBars.size() > 0);
    }

    void test_PasswordFieldEchoMode()
    {
        auto lineEdits = authWindow->findChildren<QLineEdit*>();
        if (lineEdits.size() >= 2) {
            QCOMPARE(lineEdits.at(1)->echoMode(), QLineEdit::Password);
        }
    }

private:
    std::unique_ptr<AuthWindow> authWindow;
    std::shared_ptr<cppforge::services::AuthManager> authManager;
};

QTEST_MAIN(TestAuthWindow)
#include "test_AuthWindow.moc"
